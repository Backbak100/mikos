//read CNTFRQ_EL0

.global get_timer_frequency
.global get_timer_time

get_timer_frequency:
    mrs x0, CNTFRQ_EL0
    ret

get_timer_time:
    mrs x0, CNTPCT_EL0
    ret