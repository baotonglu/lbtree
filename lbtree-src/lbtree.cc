#include "lbtree.h"

int main (int argc, char *argv[])
{
    //printf("NON_LEAF_KEY_NUM= %d, LEAF_KEY_NUM= %d, nonleaf size= %lu, leaf size= %lu\n",
           NON_LEAF_KEY_NUM, LEAF_KEY_NUM, sizeof(bnode), sizeof(bleaf));
    //assert((sizeof(bnode) == NONLEAF_SIZE)&&(sizeof(bleaf) == LEAF_SIZE));

    lbtreespace::initUseful();

    // Initialize the memory pool
    my_alloc::BasePMPool::Initialize(pool_name, pool_size);
    my_alloc::BasePMPool::IncreaseAllocatorNum(); 

    //parse_command (argc, argv);
    // create the tree
    char *nvm_addr;
    my_alloc::BasePMPool::ZAllocate((void **)&nvm_addr, 4*KB);
    lbtreespace::lbtree* index = lbtreespace::initLBTree(nvm_addr, false);
      
    // first bulk load, then do the insert operation
    int keynum;
    lbtreespace::key_type* keys;
    argc--;
    argv++;
    if (strcmp(argv[0], "bulkload") == 0)
    {
      keynum = atoi (argv[1]);
      char* keyfile = argv[2];
      float bfill; sscanf (argv[3], "%f", &bfill);

      keys = new lbtreespace::key_type[keynum];
      load_binary_data(keys, keynum, keyfile);
      std::sort(keys, keys + keynum / 2,
            [](lbtreespace::key_type const& a, lbtreespace::key_type const& b) { return a < b; });
      std::cout << "start the bulkload" << std::endl;
      int level = index->bulkload (keynum / 2, keys, bfill);
      std::cout << "finish the bulkload, root is at level " << level << std::endl;
      lbtreespace::key_type start, end;
      index->check (&start, &end);
      std::cout << "the index check is good" << std::endl;
    }

    // start the insert
    lbtreespace::key_type* key = keys + (keynum / 2);
    int insert_num = keynum / 2;
    for (int i = 0; i < insert_num; ++i)
    {
      lbtreespace::key_type kk = key[i];
      index->insert (kk, (void *) (&key[i]));
    }
    std::cout << "finish the insert" << std::endl;
    // debug insert
    int found = 0;
    for (int ii=0; ii<insert_num; ii++) {
       void *p;
       int pos;
       p = index->lookup (key[ii], &pos);
       found += (pos >= 0);
    }
    std::cout << "Found = " << found << std::endl;
    //std::cout << "Node Split = " << node_split << std::endl;
    std::cout << "Used PM = " << size_of_PM << std::endl;
    std::cout << "Used DRAM = " << size_of_DRAM << std::endl;
    my_alloc::BasePMPool::get_collected();
    return 0;
}