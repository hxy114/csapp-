/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
 static void * free_list=NULL;
 static int init_size=10240;
 static int add_size=1024*12;
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
#define MINBLOCKSIZE 32
/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SETSIZE(ptr,size) *(size_t *)ptr=size;

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

static void * first_fit(size_t size);
static void * allcate_new(size_t size);
static void  move_from_free_list(void *ptr);
static void  * cure(void *ptr,size_t size);
static void push_list(void *ptr,size_t size);
static void consisten_test();
/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    free_list=NULL;
    void *ptr=mem_sbrk(init_size);
    if(ptr==(void *)-1){
        return -1;
    }
    push_list(ptr,init_size);
    //consisten_test();


    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    int newsize = ALIGN(size + 2*SIZE_T_SIZE);
    void * ptr=first_fit(newsize);
    fflush(stdout);
    if(ptr!=NULL){
        //consisten_test();
        return ptr;
    }
    ptr=allcate_new(newsize);
    if(ptr!=NULL){
        //consisten_test();
        return ptr;
    }
    return NULL;



}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    ptr=(void *)((char *)ptr-SIZE_T_SIZE);
    size_t  size=*(size_t *)ptr&-2;
    push_list(ptr,size);
    //consisten_test();
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    size_t  align_size=ALIGN(size)+2*SIZE_T_SIZE;
    size_t  ptr_size=(*(size_t *)((char *)ptr-SIZE_T_SIZE))&-2;
    if(align_size==ptr_size||(ptr_size>align_size&&ptr_size-align_size<MINBLOCKSIZE)){
        return ptr;
    }
    if(align_size<ptr_size){
        void *new_ptr=(char *)ptr-SIZE_T_SIZE;
        size_t  ret_size=ptr_size-align_size;
        void *next_ptr=(char *)new_ptr+align_size;
        *(size_t *)new_ptr=align_size|1;
        void *ptr_tail=(char *)new_ptr+align_size-SIZE_T_SIZE;
        *(size_t *)ptr_tail=align_size|1;
        push_list(next_ptr,ret_size);
        //consisten_test();
        return ptr;
    }
    size_t data_size=*(size_t *)((char *)ptr-SIZE_T_SIZE)-2*SIZE_T_SIZE;

    void *new_ptr= mm_malloc(size);
    size_t min_size=size>data_size?data_size:size;
    strncpy(new_ptr,ptr,min_size);
    mm_free(ptr);
    return new_ptr;

}


static void * first_fit(size_t size){
    void * ptr=free_list;
    while(ptr!=NULL){
        if(*(size_t *)ptr>=size){
            if(*(size_t *)ptr-size<MINBLOCKSIZE){
                size_t  new_size=*(size_t *)ptr;
                move_from_free_list(ptr);
                *(size_t *)ptr=new_size|1;
                *(size_t *)((char *)ptr+new_size-SIZE_T_SIZE)=new_size|1;
                return (void *)((char *)ptr + SIZE_T_SIZE);

            }else{
                ptr=cure(ptr,size);
                *(size_t *)ptr = size|1;
                *(size_t *)((char *)ptr+size-SIZE_T_SIZE)=size|1;
                return (void *)((char *)ptr + SIZE_T_SIZE);
            }
        }else{

            ptr=*(void **)((char*)ptr + SIZE_T_SIZE+ sizeof(char *));
        }
    }
    return NULL;

}

static void * allcate_new(size_t size){

    void *ptr= mem_sbrk(size*2);
    push_list(ptr,size*2);
    return first_fit(size);

}
static void push_list(void *ptr,size_t size){
    void * root_ptr=ptr;
    void *pre_ptr=(char *)ptr -SIZE_T_SIZE;
    void *next_ptr=(char *)ptr+size;
    if(pre_ptr>=mem_heap_lo()&&(*(size_t *)pre_ptr&1)==0){
        size+=*(size_t *)pre_ptr;
        root_ptr=(void *)((char *)ptr-*(size_t *)pre_ptr);
        void ** pre=(void **)((char*)root_ptr+SIZE_T_SIZE);
        void ** next=(void **)((char *)pre+sizeof (char *));
        void *pre_node=*pre;
        void *next_node=*next;
        if(free_list==root_ptr){
            free_list=next_node;
        }
        if(pre_node!=NULL){
            void ** pre_node_next=(void **)((char*)pre_node+SIZE_T_SIZE+sizeof (char *));
            *pre_node_next=next_node;
        }
        if(next_node!=NULL){
            void ** next_node_pre=(void **)((char*)next_node+SIZE_T_SIZE);
            *next_node_pre=pre_node;
        }
    }

    if(next_ptr<=mem_heap_hi()&&(*(size_t *)next_ptr&1)==0){
        size+=*(size_t *)next_ptr;
        void ** pre=(void **)((char*)next_ptr+SIZE_T_SIZE);
        void ** next=(void **)((char *)pre+sizeof (char *));
        void *pre_node=*pre;
        void *next_node=*next;
        if(free_list==next_ptr){
            free_list=next_node;
        }
        if(pre_node!=NULL){
            void ** pre_node_next=(void **)((char*)pre_node+SIZE_T_SIZE+sizeof (char *));
            *pre_node_next=next_node;
        }
        if(next_node!=NULL){
            void ** next_node_pre=(void **)((char*)next_node+SIZE_T_SIZE);
            *next_node_pre=pre_node;
        }
    }
    *(size_t *)root_ptr=size;
    void *tail_ptr=(char *)root_ptr+size-SIZE_T_SIZE;
    *(size_t *)tail_ptr=size;
    void **root_ptr_pre=(void **)((char*)root_ptr+SIZE_T_SIZE);
    *root_ptr_pre=NULL;
    void **root_ptr_next=(void **)((char *)root_ptr_pre+sizeof (char *));
    *root_ptr_next=NULL;

    if(free_list!=NULL){
        void **free_list_pre=(void **)((char*)free_list+SIZE_T_SIZE);
        *root_ptr_next=free_list;
        *free_list_pre=root_ptr;
    }
    free_list=root_ptr;



}


static void  move_from_free_list(void *ptr){
    void ** pre=(void **)((char*)ptr+SIZE_T_SIZE);
    void ** next=(void **)((char *)pre+sizeof (char *));
    if(free_list==ptr){
        free_list=*next;
    }
    if(*pre!=NULL){
        void *pre_node=*pre;
        void ** pre_node_next=(void **)((char *)pre_node+SIZE_T_SIZE+sizeof (char *));
        *pre_node_next=*next;
    }
    if(*next!=NULL){
        void *next_node=*next;
        void ** next_node_pre=(void **)((char *)next_node+SIZE_T_SIZE);
        *next_node_pre=*pre;
    }


}
static void *  cure(void *ptr,size_t size){
    size_t total_size=*(size_t *)ptr,re_size=total_size-size;
    *(size_t *)ptr=re_size;
    void *tail_ptr=(char *)ptr+re_size-SIZE_T_SIZE;
    *(size_t *)tail_ptr=re_size;
    return (void *)((char *)ptr+re_size);

}


static void consisten_test(){
    void * ptr=free_list;
    printf("----------------------------------\n");
    printf("空闲空间检查\n");
    while(ptr!=NULL){
        void *tail=(void *)((char *)ptr+*(size_t *)ptr-SIZE_T_SIZE);
        size_t  size=*(size_t *)ptr&-2;
        size_t index=*(size_t *)ptr&1;
        void *pre=*(void **)((char*)ptr + SIZE_T_SIZE);
        void *next =*(void **)((char*)ptr + SIZE_T_SIZE+ sizeof(char *));
        printf("[start:%p   end:%p  pre:%p  next:%p  size:%d  index:%d]\n",ptr,tail,pre,next,size,index);
        ptr=next;
    }
    printf("全部空间检查:hi=%p\n",mem_heap_hi());
    ptr=mem_heap_lo();

    while(ptr<=mem_heap_hi()){
        size_t  size=*(size_t *)ptr&-2;

        size_t index=*(size_t *)ptr&1;
        void *tail=(void *)((char *)ptr+size-SIZE_T_SIZE);
        size_t  tail_size=*(size_t *)tail&-2;
        size_t tail_index=*(size_t *)tail&1;
        void *next =(void *)((char*)ptr + size);
        printf("[start:%p   end:%p   next:%p  size:%d tail_size:%d index:%d  tail_index:%d]\n",ptr,tail,next,size,tail_size,index,tail_index);
        ptr=next;
    }
    printf("----------------------------------\n");

}



