/*
 * Copyright 2019-2022 RnD Center "ELVEES", JSC
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __MAILBOX_H__
#define __MAILBOX_H__

#define MAILBOX_FIFO_COUNT 8UL
#define MAILBOX_FIFO_STATUS_FULL BIT(0)
#define MAILBOX_FIFO_STATUS_EMPTY BIT(1)

/**
 * @brief Mailbox state machine
 *
 */
typedef enum {
	MAILBOX_STATE_LOCKED,
	MAILBOX_STATE_UNLOCKED,
} mailbox_sm_t;

/**
 * @brief Structure of the Mailbox registers
 *
 */
typedef struct {
	struct {
		volatile unsigned int message;
		volatile unsigned int fifo_status;
		volatile unsigned int reserved0[2];
		volatile unsigned int reserved1;
		volatile unsigned int set_irq_read;
		volatile unsigned int reserved2;
		volatile unsigned int clr_irq_read;
		volatile unsigned int reserved3[1016];
	} mailbox[MAILBOX_FIFO_COUNT];
	volatile unsigned int soft_reset;
	volatile unsigned int mbox_version;
} mailbox_regs_t;

/**
 * @brief Initialize mailbox
 *
 * @param mailbox_base - Base address
 *
 * @return none
 */
void mailbox_init(void *mailbox_base);

/**
 * @brief Prepare mailbox for command transmission.
 *
 * This is blocking function.
 * This function should be used at the beginning of any transmission.
 *
 * @param fifo_num FIFO number of the Mailbox
 * @param timeout  Timeout in ms. If timeout == 0,
 *                 the function will wait in infinity loop
 *
 * @return 0     - Mailbox is ready,
 *         ERRNO - In case of error
 */
int mailbox_message_start(uint32_t fifo_num, uint32_t timeout);

/**
 * @brief Write message to mailbox FIFO
 *
 * This is blocking function.
 *
 * @param fifo_num FIFO number of the Mailbox
 * @param data     Pointer to the data to be written
 * @param size     Size of data in bytes
 * @param timeout  Timeout in ms. If timeout == 0,
 *                 the function will wait in infinity loop
 *
 * @return size  - Num of written bytes,
 *         ERRNO - In case of error
 */
int mailbox_message_write(uint32_t fifo_num, const uint8_t *data,
			  uint32_t size, uint32_t timeout);

/**
 * @brief Finish mailbox command transmission.
 *
 * This function should be used at the ending of any transmission.
 *
 * @return none
 */
void mailbox_message_end(void);

/**
 * @brief Notify the receiving party
 *
 * @param fifo_num FIFO number of the Mailbox
 *
 * @return 0 - Success, ERRNO - In case of error
 */
int mailbox_notify(uint32_t fifo_num);

#endif /* __MAILBOX_H__ */
