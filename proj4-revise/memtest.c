
#include "types.h"
#include "user.h"


int
main(int argc, char *argv[])
{
          
    char * cur_break = (void *)0;
    char * old_break = (void *)0;
    int pre, post;

    printf(1,"number of free frames: %d \n",get_free_frame_cnt());

      
    printf(1,"Test 1 (sbrk without allocating memory): ");
    old_break=sbrk(0);
    pre=get_free_frame_cnt();
    sbrk(4096*4);//four pages
    cur_break=sbrk(0);
    post=get_free_frame_cnt();
    if(cur_break-old_break<4096*4){
        printf(1," fail1! %d \n",cur_break-old_break);
    }else if(post<pre){
        printf(1," fail2!\n");
    }else{
        printf(1," success!\n");
    }

    printf(1,"Test 2 (write to heap mem): ");
    *(cur_break-1)='c';
    post=get_free_frame_cnt();
    if(pre>post+2){//allocate at most two frames
        printf(1," fail!\n");
    }else{
        printf(1," success!\n");
    }

    printf(1,"Test 3 (deallocating memory): ");
    sbrk(-4*1024);
    cur_break=sbrk(0);
    post=get_free_frame_cnt();
    if(pre-post>1){
        printf(1," fail!\n");
    }else{
        printf(1," success!\n");
    }

    printf(1,"Test 4 (allocating too much mem): ");
    old_break=sbrk(0);
    sbrk(0x7FFFFFFF);
    cur_break=sbrk(0);
    if(cur_break!=old_break){
        printf(1," fail!\n");
    }else{
        printf(1," success!\n");
    }

    printf(1,"Test 5 (access invalid mem, two page faults): \n" );
    if(fork()==0){
        *(cur_break+4096)='c';
        printf(1," test failure 1!\n");
        exit();
    }else{
        wait();
    }    
    if(fork()==0){
        int i=0;        
        while(cur_break){
            cur_break--;
            if(*(cur_break)==0){
            //read memory at cur_break
                // should cause page fault below stack
                i++;
            }else{
                i++;
            }
        }
        printf(1,"test failure 2!\n");
        exit();
    }else{
        wait();
    }
    exit();
}

