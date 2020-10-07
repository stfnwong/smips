/*
 * DATACACHE
 */

#ifndef __DATACACHE_HPP
#define __DATACACHE_HPP

#define SMIPS_MEM_SIZE 0x100000       // TODO: boost this up later


class DataCache
{
    private:
        uint8_t* mem;
        unsigned int mem_size;

    public:
        DataCache();
        DataCache(unsigned int size);
        DataCache(const DataCache& that);
        ~DataCache();
        unsigned int size(void) const;
        void clear(void);       // write all zeros (slow!)

        uint8_t &operator[](const int i);  
        int32_t readWord(const int i) const;     // read 4-bytes starting from i;;
};



#endif /*__DATACACHE_HPP*/
