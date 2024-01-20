
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
	PROCESS* p;
	int	 inf = 0xfffffff;
	int	 greatest_priority = 0xfffffff;

	while (greatest_priority == inf) {
		for (p = proc_table; p < proc_table+NR_TASKS; p++) {
			if (p->priority < greatest_priority) {
				greatest_priority = p->priority;
				p_proc_ready = p;
			}
		}

		// if (!greatest_ticks) {
		// 	for (p = proc_table; p < proc_table+NR_TASKS; p++) {
		// 		p->ticks = p->priority;
		// 	}
		// }
	}
}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}

