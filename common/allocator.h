#pragma once

#include <cstddef>
#include <climits>
#include <cstdlib>
#include <new>
#include <vector>
#include <string>
#include <sys/stat.h>

#include "libpmem.h"
#include "libpmemobj.h"

#include "utils.h"

// In this class, I will write a custom template allocator
// Specifically, it allocates persistent memory using PMDK interface
// Moreover, need to use static member to make all allocatoion in a single memory pool

static const char* layout_name = "template_pool";
static const uint64_t pool_addr = 0x5f0000000000;
static const char* pool_name = "/mnt/pmem0/baotong/lbtree.data";
static const uint64_t pool_size = 20UL * 1024*1024*1024;
 
//#define PREALLOC 1

namespace my_alloc{
	template <class T1, class T2>
	inline void _construct(T1* p, const T2& value){new (p) T1(value);}

	template <class T>
	inline void _destroy(T* ptr){ ptr->~T();}

    //Implement a base class that has the memory pool
class BasePMPool{
public:
    static PMEMobjpool *pm_pool_;
    static int allocator_num;
    static const uint64_t kAllTables = 4UL * 1024 * 1024 * 1024;    
    static PMEMoid p_all_tables;
    static char *all_tables;
    static uint64_t all_allocated;
    static uint64_t all_deallocated;
    static uint64_t collect_allocated;

    static void Initialize(const char* pool_name, size_t pool_size){
        if (!FileExists(pool_name)) {
            LOG("creating a new pool");
            //pm_pool_ = pmemobj_create_addr(pool_name, layout_name, pool_size,
            //                                CREATE_MODE_RW, (void*)pool_addr);
            pm_pool_ = pmemobj_create(pool_name, layout_name, pool_size,
                                            CREATE_MODE_RW);
            if (pm_pool_ == nullptr) {
                LOG_FATAL("failed to create a pool;");
            }
            //std::cout << "pool opened at: " << std::hex << pm_pool_ << std::dec << std::endl;
        }else{
            LOG("opening an existing pool, and trying to map to same address");
            /* Need to open an existing persistent pool */
            //pm_pool_ = pmemobj_open_addr(pool_name, layout_name, (void*)pool_addr);
            pm_pool_ = pmemobj_open(pool_name, layout_name);
            if (pm_pool_ == nullptr) {
                LOG_FATAL("failed to open the pool");
            }
            std::cout << "pool opened at: " << std::hex << pm_pool_
                << std::dec << std::endl;
        }
    }

    static void IncreaseAllocatorNum(){
        allocator_num++;
    }

    static void DecreaseAllocatorNum(){
        allocator_num--;
    }

    static void ClosePool(const char* pool_name){
        std::cout << "remove the memory pool" << std::endl;
        if(pm_pool_ != nullptr){
        pmemobj_close(pm_pool_);
        pm_pool_ = nullptr;
        //FIXME: should not delete the memory pool, but for easy test, I add this
        remove(pool_name);        
        }
    }

    static void Allocate(void** ptr, size_t size){
        PMEMoid tmp_ptr;
        auto ret = pmemobj_alloc(pm_pool_, &tmp_ptr, size + 64, TOID_TYPE_NUM(char), NULL, NULL);
        if (ret) {
          std::cout << "Fail logging: " << ret << "; Size = " << size << std::endl;
          LOG_FATAL("Allocate: Allocation Error in PMEMoid 1");
        }
        //*ptr = pmemobj_direct(tmp_ptr);
        uint64_t ptr_value = (uint64_t)(pmemobj_direct(tmp_ptr)) + 48;
        *ptr = (void*)(ptr_value);
        //collect_allocated++;
    }

    static void get_collected(){ 
        std::cout << "I have allocate blocks " << collect_allocated << std::endl;
    }

    static void ZAllocate(void** ptr, size_t size){
        PMEMoid tmp_ptr;
        auto ret = pmemobj_zalloc(pm_pool_, &tmp_ptr, size, TOID_TYPE_NUM(char));
        if (ret) {
          std::cout << "Fail logging: " << ret << "; Size = " << size << std::endl;
          LOG_FATAL("Allocate: Allocation Error in PMEMoid 1");
        }
        //*ptr = pmemobj_direct(tmp_ptr);
         uint64_t ptr_value = (uint64_t)(pmemobj_direct(tmp_ptr)) + 48;
        *ptr = (void*)(ptr_value);
    }


    static void Allocate(PMEMoid *ptr, size_t size){
        auto ret = pmemobj_alloc(pm_pool_, ptr, size, TOID_TYPE_NUM(char), NULL, NULL);
        //auto ret = pmemobj_zalloc(pm_pool_, ptr, size, TOID_TYPE_NUM(char));
        if (ret) {
          std::cout << "Fail logging: " << ret << "; Size = " << size << std::endl;
          LOG_FATAL("Allocate: Allocation Error in PMEMoid 1");
        }
    }


    static void ZAllocate(PMEMoid *ptr, size_t size){
        auto ret = pmemobj_zalloc(pm_pool_, ptr, size, TOID_TYPE_NUM(char));
        if (ret) {
          std::cout << "Fail logging: " << ret << "; Size = " << size << std::endl;
          LOG_FATAL("Allocate: Allocation Error in PMEMoid 1");
        }
    }

    static void PREAllocate(void **ptr, size_t size){
        if((all_allocated + size) > kAllTables){
            printf("Exceed prealocate limit\n");
            abort();
        }

        *ptr = reinterpret_cast<void *>(all_tables + all_allocated);
        all_allocated += size;
    }

    static void PREFree(){
        all_deallocated++;
    }

	static void Free(void* p){
        uint64_t ptr_value = (uint64_t)(p) - 48;
        p = (void*)(ptr_value); 
        auto ptr = pmemobj_oid(p);
        pmemobj_free(&ptr);
    }

    static void Persist(void* p, size_t size){
        pmemobj_persist(pm_pool_, p, size);
    }
};

	
template <class T>
class allocator : BasePMPool{
public: 
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	//constructor fucn
    allocator(){
        std::cout << "Intial allocator: " << allocator_num << std::endl;
        allocator_num++;
        if(allocator_num == 1){
            BasePMPool::Initialize(pool_name, pool_size);
        }
    }

    allocator(const allocator<T>& c){
       // std::cout << "Copy allocator: " << allocator_num << std::endl;
        allocator_num++;
        if(allocator_num == 1){
            BasePMPool::Initialize(pool_name, pool_size);
        }
    }

    ~allocator(){
        allocator_num--;
        if(allocator_num == 0){
            //delete PM pool
            ClosePool(pool_name);
        }
    }

	template <class U>
	allocator(const allocator<U>& c){
        //std::cout << "Other type allocator" << std::endl;
        allocator_num++;
        if(allocator_num == 1){
            BasePMPool::Initialize(pool_name, pool_size);
        }
    }

	//rebind allocator of type U
	template <class U>
	struct rebind {typedef allocator<U> other;};

	//pointer allocate(size_type n, const void* hint=0){
    pointer allocate(size_type n){
        //FIXME: non-safe memory allocation
        PMEMoid tmp_ptr;
        auto ret = pmemobj_alloc(pm_pool_, &tmp_ptr, (size_t)(n * sizeof(T)), TOID_TYPE_NUM(char), NULL, NULL);
        //auto ret = pmemobj_zalloc(pm_pool_, &tmp_ptr, (size_t)(n * sizeof(T)), TOID_TYPE_NUM(char));
        if (ret) {
          std::cout << "Fail logging: " << ret << "; Size = " << n *sizeof(T) << std::endl;
          LOG_FATAL("Allocate: Allocation Error in PMEMoid 2");
        }
        pointer tmp = (pointer)pmemobj_direct(tmp_ptr);
        return tmp;
	}

    //PM allocator interface, and initialization
    /*
    void allocate(void** ptr, uint32_t alignmnent, size_t size, int (*alloc_constr)(PMEMobjpool* pool, void* ptr, void* arg), void* arg){
        TX_BEGIN(pm_pool_){
            pmemobj_tx_add_range_direct(ptr, sizeof(*ptr));
            *ptr = pmemobj_direct(pmemobj_tx_alloc(size, 100));
            alloc_constr(pm_pool_, *ptr, arg);
        }
        TX_ONABORT{LOG_FATAL("Allocation: TXN Allocation Error!");}
        TX_END
    }*/
	void deallocate(pointer p, size_type n){ 
        auto ptr = pmemobj_oid(p);
        pmemobj_free(&ptr);
    }
	void construct(pointer p, const T& value){ _construct(p, value);}
	void destroy(pointer p){_destroy(p);}

	pointer address(reference x){return (pointer)&x;}
	const_pointer const_address(const_reference x){ return (const_pointer)&x;}

	size_type max_size() const {return size_type(UINT_MAX / sizeof(T));}
};

PMEMobjpool* BasePMPool::pm_pool_ = nullptr;
int BasePMPool::allocator_num = 0;
PMEMoid BasePMPool::p_all_tables = OID_NULL;
char* BasePMPool::all_tables = nullptr;
uint64_t BasePMPool::all_allocated = 0;
uint64_t BasePMPool::all_deallocated = 0;
uint64_t BasePMPool::collect_allocated = 0;
}