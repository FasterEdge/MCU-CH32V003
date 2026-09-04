// FasterEdge 开源项目 - Github: https://github.com/FasterEdge - Gitee: https://gitee.com/FasterEdge
// ability_time.c — TimeAbility 实现（C51/8051 版）
// sync_manual / sync_system / get_time / configure_run
// C51 无网络，不含 sync_ntp/sync_net。时间源由 fe_port 抽象
// （RTC 芯片 / 定时器计数）。epoch 用 u32（到 2106 年够用）。
#include "fe_ability.h"
#include "fe_port.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

fe_output_t ability_time_dispatch(void *inst, const char *act, const char *args) {
    time_ability_t *self = (time_ability_t *)inst;

    if (strcmp(act, "get_time") == 0) {
        u32 now = fe_port_time_now();
        char out[48];
        fe_snprintf(out, sizeof(out), "{\"epoch\":%lu}", (unsigned long)now);
        return fe_ok(act, out);
    }
    if (strcmp(act, "sync_manual") == 0) {
        if (!args || args[0] == 0 || args[0] == '-')
            return fe_err(act, "invalid epoch");
        // strtoul 溢出(如 "9999999999")会回绕成 ULONG_MAX 且看似合法,
        // 负数字符串("-5")也会回绕成接近 ULONG_MAX 的值被接受;
        // 必须检查 ERANGE 与结束指针, 拒绝一切非法/越界输入。
        char *endp = NULL;
        errno = 0;
        unsigned long raw = strtoul(args, &endp, 10);
        if (errno == ERANGE || *endp != 0)
            return fe_err(act, "invalid epoch");
        u32 ep = (u32)raw;
        if (ep == 0) return fe_err(act, "invalid epoch");
        fe_port_time_set(ep);
        self->manual_epoch = ep;
        char out[48];
        fe_snprintf(out, sizeof(out), "epoch=%lu", (unsigned long)ep);
        return fe_ok(act, out);
    }
    if (strcmp(act, "sync_system") == 0) {
        u32 now = fe_port_time_now();
        char out[48];
        fe_snprintf(out, sizeof(out), "epoch=%lu", (unsigned long)now);
        return fe_ok(act, out);
    }
    if (strcmp(act, "configure_run") == 0) {
        u32 interval;
        char out[80];
        if (!args || !args[0]) {
            fe_snprintf(out, sizeof(out),
                        "{\"enabled\":%s,\"interval\":%lu,\"nextRun\":%lu}",
                        self->run_enabled ? "true" : "false",
                        (unsigned long)self->run_interval,
                        (unsigned long)self->next_run);
            return fe_ok(act, out);
        }
        interval = (u32)strtoul(args, NULL, 10);
        if (interval == 0) {
            self->run_enabled = FALSE;
            self->run_interval = 0;
            self->next_run = 0;
        } else {
            self->run_enabled = TRUE;
            self->run_interval = interval;
            // u32 加法回绕: now 接近 0xFFFFFFFF 时 now+interval 回绕成 0,
            // 定时任务将永远不再触发(直到 epoch 再绕一圈)。饱和到最大值。
            u32 now = fe_port_time_now();
            self->next_run = (interval > 0xFFFFFFFFUL - now) ? 0xFFFFFFFFUL : now + interval;
        }
        fe_snprintf(out, sizeof(out),
                    "{\"enabled\":%s,\"interval\":%lu,\"nextRun\":%lu}",
                    self->run_enabled ? "true" : "false",
                    (unsigned long)self->run_interval,
                    (unsigned long)self->next_run);
        return fe_ok(act, out);
    }
    return fe_err(act, "unsupported command");
}
