/*
 * Copyright 2019-2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <common/debug.h>
#include <lib/mmio.h>
#include <lib/bakery_lock.h>
#include <drivers/delay_timer.h>
#include "mailbox.h"

#define mailbox_lock_init() bakery_lock_init(&mailbox_lock)
#define mailbox_lock_get() bakery_lock_get(&mailbox_lock)
#define mailbox_lock_release() bakery_lock_release(&mailbox_lock)

#define mailbox_read_reg(num, reg) \
	mmio_read_32((uintptr_t)&base->mailbox[num].reg)
#define mailbox_write_reg(num, reg, value) \
	mmio_write_32((uintptr_t)&base->mailbox[num].reg, (value))

static DEFINE_BAKERY_LOCK(mailbox_lock);
static mailbox_regs_t *base;
static mailbox_sm_t sm;

/**
 * @brief Waiting for mailbox FIFO to be free.
 *
 * This is blocking function.
 * This function uses mailbox_regs_t::set_irq_read as semaphore
 * to understand when mailbox will be free and all messages
 * are handled before sending new ones.
 *
 * @param fifo_num FIFO number of the Mailbox
 * @param timeout  Timeout in ms. If timeout == 0,
 *                 the function will wait in infinity loop
 *
 * @return 0     - Mailbox is ready,
*          ERRNO - In case of error
 */
static int mailbox_message_wait(uint32_t fifo_num, uint32_t timeout)
{
	uint64_t expire;

	if (!base)
		return -EINVAL;

	if (fifo_num >= MAILBOX_FIFO_COUNT)
		return -EINVAL;

	if (timeout)
		expire = timeout_init_us(timeout * 1000U);

	// TODO: Maybe this is redundant check.
	/* Wait until the outbound mailbox becomes empty */
	while ((mailbox_read_reg(fifo_num, fifo_status) & MAILBOX_FIFO_STATUS_EMPTY) == 0U) {
		if (timeout)
			if (timeout_elapsed(expire))
				return -ETIMEDOUT;
	}

	/* Wait while the outbound mailbox is busy */
	while (mailbox_read_reg(fifo_num, set_irq_read) != 0U) {
		if (timeout)
			if (timeout_elapsed(expire))
				return -ETIMEDOUT;
	}

	return 0;
}

void mailbox_init(void *mailbox_base)
{
	base = (mailbox_regs_t *)mailbox_base;
	sm = MAILBOX_STATE_UNLOCKED;
	mailbox_lock_init();
}

int mailbox_message_start(uint32_t fifo_num, uint32_t timeout)
{
	if (fifo_num >= MAILBOX_FIFO_COUNT)
		return -EINVAL;

	mailbox_lock_get();
	dmbsy();
	sm = MAILBOX_STATE_LOCKED;

	return mailbox_message_wait(fifo_num, timeout);
}

int mailbox_message_write(uint32_t fifo_num, const uint8_t *data,
			  uint32_t size, uint32_t timeout)
{
	uint32_t has_written = 0U;
	uint32_t fifo_word = 0U;
	uint32_t bytes_to_write = 0U;
	uint64_t expire;

	if (fifo_num >= MAILBOX_FIFO_COUNT)
		return -EINVAL;

	if (!base)
		return -EINVAL;

	if (!data)
		return -EINVAL;

	if (sm != MAILBOX_STATE_LOCKED)
		return -EPERM;

	if (!size)
		return (int)has_written;

	if (timeout)
		expire = timeout_init_us(timeout * 1000U);

	bytes_to_write = sizeof(fifo_word);
	while (size > 0U) {
		if ((mailbox_read_reg(fifo_num, fifo_status) & MAILBOX_FIFO_STATUS_FULL) == 0U) {
			if (bytes_to_write > size)
				bytes_to_write = size;
			fifo_word = 0U;
			memcpy((void *)&fifo_word, (void *)data, bytes_to_write);
			mailbox_write_reg(fifo_num, message, fifo_word);
			dmbsy();
			data += bytes_to_write;
			size -= bytes_to_write;
			has_written += bytes_to_write;
		}

		if (timeout)
			if (timeout_elapsed(expire))
				return -ETIMEDOUT;
	}

	return (int)has_written;
}

void mailbox_message_end(void)
{
	sm = MAILBOX_STATE_UNLOCKED;
	dmbsy();
	mailbox_lock_release();
}

int mailbox_notify(uint32_t fifo_num)
{
	if (fifo_num >= MAILBOX_FIFO_COUNT)
		return -EINVAL;

	if (!base)
		return -EINVAL;

	if (sm != MAILBOX_STATE_LOCKED)
		return -EPERM;

	mailbox_write_reg(fifo_num, set_irq_read, 0x1U);

	return 0;
}
