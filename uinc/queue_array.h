#ifndef _QUEUE_ARRAY
#define _QUEUE_ARRAY

// #define QueueDeep 40
// #include "type.h"
#define CREATE_ARRAY_QUEUE(QUEUE_TYPE,SUFFIX,QUEUE_SIZE)																                                \
	typedef struct																											                                                  \
	{																														                                                          \
		QUEUE_TYPE Q##SUFFIX[QUEUE_SIZE];			     																	                                        \
		unsigned int         size;																									                                                  \
		unsigned int         location;																								                                                \
	}QUEUE##SUFFIX;					                                   							        			                                \
	QUEUE##SUFFIX queue##SUFFIX  = {0};		 						                                      \
	QUEUE_TYPE back##SUFFIX( void )																							                                          \
	{																														                                                          \
		QUEUE_TYPE TNull = {0};																								                                              \
		return queue##SUFFIX.size > 0 ? queue##SUFFIX.Q##SUFFIX[queue##SUFFIX.location + queue##SUFFIX.size - 1] : TNull;   \
	}																														                                                          \
	QUEUE_TYPE front##SUFFIX( void )																						                                          \
	{																														                                                          \
		QUEUE_TYPE TNull = {0};																								                                              \
		return queue##SUFFIX.size > 0 ? queue##SUFFIX.Q##SUFFIX[queue##SUFFIX.location] : TNull;					     	            \
	}																														                                                          \
	void    pop##SUFFIX( void )	  																							                                          \
	{																														                                                          \
    if(queue##SUFFIX.size > 0)	    																						                                        \
		{																													                                                          \
            --queue##SUFFIX.size    ;																						                                        \
            queue##SUFFIX.location = (++queue##SUFFIX.location)%QUEUE_SIZE;											                        \
		}																												                                                            \
	}																														                                                          \
	unsigned char      is_full##SUFFIX  ( void )													    									                                      \
	{																														                                                          \
        return QUEUE_SIZE == queue##SUFFIX.size;																			                                  \
	}																														                                                          \
	void    push##SUFFIX  ( QUEUE_TYPE PS )																					                                      \
	{																														                                                          \
    if(!is_full##SUFFIX())																								                                              \
		{																													                                                          \
            										                             										                                        \
		}else{                                                                                                              \
			  pop##SUFFIX();                                                                                                  \
		}                                                                                                                   \
		queue##SUFFIX.Q##SUFFIX[(queue##SUFFIX.location + queue##SUFFIX.size)%QUEUE_SIZE] = PS;							                \
    ++queue##SUFFIX.size    ;			                                                                                    	\
	}																														                                                          \
	unsigned int      size##SUFFIX  ( void )																							                                          \
	{																														                                                          \
        return queue##SUFFIX.size;																							                                        \
	}	 																													                                                          \
	unsigned char is_empty##SUFFIX( void )																								                                            \
	{																														                                                          \
		return queue##SUFFIX.size==0;                                                                                       \
	}                                                                                                                     \
	
#endif
