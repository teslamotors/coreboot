#ifndef _MBOX_H_
#define _MBOX_H_

#include <stdint.h>
//#include <stdbool.h>

struct mbox_int {
  uint32_t intgr;
  uint32_t intcr;
  uint32_t intmr;
  uint32_t intsr;
  uint32_t intmsr;
};

#define MAILBOX_REGS_PER_BLOCK 16
#define MAX_REQUEST_ARGUMENTS (MAILBOX_REGS_PER_BLOCK - 3)

struct mbox_issr {
    uint32_t   issr[MAILBOX_REGS_PER_BLOCK];
};

struct mbox_semaphore {
    uint32_t   semaphore[3];
};

struct mbox {
    uint32_t mcu_ctrl;

    uint32_t rsvd;
    struct mbox_int int0;
    struct mbox_int int1;
    uint32_t rsvd2[(0x50-0x30)/4];
    uint32_t version;
    uint32_t rsvd3[(0x80-0x54)/4];

    struct mbox_issr issr_lo0;
    struct mbox_issr issr_lo1;
    struct mbox_issr issr_hi0;
    struct mbox_issr issr_hi1;

    struct mbox_semaphore  _semaphores_lo0;
    struct mbox_semaphore  _semaphores_lo1;
    struct mbox_semaphore  _semaphores_hi0;
    struct mbox_semaphore  _semaphores_hi1;
};

#define ISSR_INDEX_DATA0   0
#define ISSR_INDEX_DATA1   1
#define ISSR_INDEX_DATA2   2
#define ISSR_INDEX_DATA3   3
#define ISSR_INDEX_DATA4   4
#define ISSR_INDEX_DATA5   5
#define ISSR_INDEX_DATA6   6
#define ISSR_INDEX_DATA7   7
#define ISSR_INDEX_DATA8   8
#define ISSR_INDEX_DATA9   9
#define ISSR_INDEX_DATA10  10
#define ISSR_INDEX_DATA11  11
#define ISSR_INDEX_DATA12  12
#define ISSR_INDEX_REQUEST 13
#define ISSR_INDEX_STATUS  14
#define ISSR_INDEX_SERVICE 15

#ifdef MBOX_COMMAND_RESPONSE_ID
#undef MBOX_COMMAND_RESPONSE_ID
#endif
#define MBOX_COMMAND_RESPONSE_ID 0x80000000

#define NN_MAX		12
#define RSP_NN_MAX	11

struct trav_mbox {
	struct mbox_issr *issr_tx;
	struct mbox_issr *issr_rx;
	struct mbox_int *mbox_int_rx;
	struct mbox_int *mbox_int_tx;
	int pri;
};

int mailbox_send_request(struct trav_mbox *mbox, uint32_t service_id, uint32_t request_id, uint32_t argc,
                         const uint32_t * args, uint32_t * response_status, uint32_t nn, uint32_t * return_value);
int mailbox_wait_for_response(struct trav_mbox *mbox, uint32_t request_id, uint32_t * response_status,
                              uint32_t expected_response_code, uint32_t * return_value, uint32_t return_nn,
                              int (*mailbox_handle_request)(struct trav_mbox *, uint32_t, uint32_t));
extern uint32_t request_id_counter;
int test_op_nn_with_nn(struct trav_mbox *mbox, uint32_t service_id,
                                 uint32_t cmd, uint32_t *cmd_arg, int nn,
                                 uint32_t *return_arg, int return_nn);

int test_op_nn_with_nn_async(struct trav_mbox *mbox, uint32_t service_id,
                                 uint32_t cmd, uint32_t *cmd_arg, int nn,
                                 uint32_t resp, uint32_t *return_arg, int return_nn);

void check_build_git_hash(struct trav_mbox *mbox, const char *name, uint32_t cmd, char *build_git_hash);

#endif
