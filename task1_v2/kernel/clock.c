/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	                               clock.c
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
                          clock_handler
*======================================================================*/
PUBLIC void clock_handler(int irq)
{
	ticks++;
	p_proc_ready->ticks--;
	PROCESS* p;

	//disp_int(p_proc_ready->pid);
	//if (k_reenter != 0) {
	// return;
	//}

	if(p_proc_ready->priority == 1)
	{
		if(p_proc_ready->ticks == 0)
		{
	      	p_proc_ready->priority = 2;
	      	p_proc_ready->ticks = time_slice * 2;
      		schedule();
  		}
 	}else if(p_proc_ready->priority == 2){
	  	if(p_proc_ready->ticks != 0)
	  	{
			for(p = proc_table; p < proc_table + NR_TASKS; p++)
			{
				if (p->priority == 1)
				{
					p_proc_ready = p;
					return;
				}
			}
			for(p = proc_table; p < proc_table + NR_TASKS; p++)
			{
				if (p->priority == 2)
				{
					p_proc_ready = p;
					return;
				}
			}
	  	}else{
	     	p_proc_ready->priority = 3;
	      	p_proc_ready->ticks = time_slice * 3;
	      	schedule();
 		}
	}else if(p_proc_ready->priority == 3){
	  	if(p_proc_ready->ticks != 0)
		{
      		for(p = proc_table; p < proc_table + NR_TASKS; p++)
			{
	   			if (p->priority == 1)
				{
	       			p_proc_ready = p;
       				return;
   				}
      		}
	      	for(p = proc_table; p < proc_table + NR_TASKS; p++)
			{
	   			if (p->priority == 2)
				{
	       			p_proc_ready = p;
	       			return;
	   			}
	      	}
  		}else{
	   		p_proc_ready->ticks = time_slice * 3;
	   		schedule();
	   		if(p_proc_ready->priority == 3)
			{
	    		while(1){
	     			process_num = (process_num + 1) % NR_TASKS;
     				p_proc_ready = proc_table + process_num;
	     			if(p_proc_ready->priority == 3) break;    
	    		}
	   		}
	  	}
	 }
	 return;
	
	 //if (p_proc_ready->ticks > 0) {
	 // return;
	 //}
	
	 //schedule();
	
	}
	
/*======================================================================*
	                              milli_delay
*======================================================================*/
PUBLIC void milli_delay(int milli_sec)
{
		int t = get_ticks();

		while(((get_ticks() - t) * 1000 / HZ) < milli_sec) {}
}
