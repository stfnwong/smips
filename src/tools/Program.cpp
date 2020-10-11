/*
 * PROGRAM
 * Represents a binary program
 *
 * Stefan Wong 2019
 */

#include "Program.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>


// Instr

Instr::Instr()
{
    this->init();
}

Instr::Instr(const uint32_t adr, const uint32_t ins)
{
    this->adr = adr;
    this->ins = ins;
}

// TODO : I suppose that there should also be a move constructor here
/*
 * copy ctor
 */
Instr::Instr(const Instr& that)
{
    this->adr = that.adr;
    this->ins = that.ins;
}

/*
 * move ctor
 */
Instr::Instr(const Instr&& that)
{
    this->adr = std::move(that.adr);
    this->ins = std::move(that.ins);
}


/*
 * ==
 */
bool Instr::operator==(const Instr& that) const
{
    if(this->adr != that.adr)
        return false;
    if(this->ins != that.ins)
        return false;

    return true;
}

/*
 * !=
 */
bool Instr::operator!=(const Instr& that) const
{
    return !(*this == that);
}

Instr& Instr::operator=(const Instr& that)
{
    this->adr = that.adr;
    this->ins = that.ins;

    return *this;
}

void Instr::init(void)
{
    this->adr = 0;
    this->ins = 0;
}


std::string Instr::toString(void) const
{
    std::ostringstream oss;

    oss << "0x" << std::hex << std::setfill('0') << std::setw(8) << this->adr;
    oss << " [" << std::setw(8) << std::setfill('0') << this->ins << "] ";

    return oss.str();
}


// DataSeg
DataSeg::DataSeg()
{
    this->adr = 0;
}

DataSeg::DataSeg(uint32_t adr, const std::vector<uint8_t> d)
{
    this->adr = adr;
    this->data = d;
}

void DataSeg::init(void)
{
    this->adr = 0;
    this->data.clear();
}

/*
 * assignment
 */
DataSeg& DataSeg::operator=(const DataSeg& that)
{
    if(*this != that)
    {
        this->data.clear();
        this->adr  = that.adr;
        this->data = that.data;
    }

    return *this;
}

/*
 * ==
 */
bool DataSeg::operator==(const DataSeg& that) const
{
    if(this->adr != that.adr)
        return false;
    if(this->data.size() != that.data.size())
        return false;

    for(unsigned int i = 0; i < this->data.size(); ++i)
    {
        if(this->data[i] != that.data[i])
            return false;
    }

    return true;
}

/*
 * !=
 */
bool DataSeg::operator!=(const DataSeg& that) const
{
    return !(*this == that);
}

/*
 * diff()
 */
std::string DataSeg::diff(const DataSeg& that) const
{
    std::ostringstream oss;
    int num_err = 0;

    // Address
    if(this->adr != that.adr)
    {
        oss << "adr [" << this->adr << "] does not match [" << that.adr << "]" << std::endl;
        num_err += 1;
    }

    // Size of segment vector
    if(this->data.size() != that.data.size())
    {
        oss << "data.size() [" << this->data.size() << "] does not match [" << that.data.size() << "]" << std::endl;
        num_err += 1;
    }

    // Segments 
    for(unsigned int d = 0; d < this->data.size(); ++d)
    {
        if(this->data[d] != that.data[d])
        {
            oss << "element (" << std::dec << d << ") in segment data [" << 
                std::hex << this->data[d] << "] does not match [" << 
                that.data[d] << "]" << std::endl;
            num_err += 1;
        }
    }

    return oss.str();
}

/*
 * toString()
 */
std::string DataSeg::toString(void) const
{
    std::ostringstream oss;

    // address
    oss << this->data.size() << " words starting at ";
    oss << "[0x" << std::hex << std::setw(8) << std::setfill('0')
        << this->adr << "]  " << std::endl;
    for(unsigned int i = 0; i < this->data.size(); ++i)
    {
        oss << std::hex << unsigned(this->data[i]) << " ";
    }

    return oss.str();
}



// ======== PROGRAM ======== //
Program::Program()
{
    this->verbose = false;
    this->init();
}

/*
 * init()
 */
void Program::init(void)
{
    this->instructions.clear();
    this->data_segments.clear();
}

/*
 * add()
 * Add an Instr
 */
void Program::add(const Instr& i)
{
    this->instructions.push_back(i);
}

/*
 * add()
 * Add a DataSeg
 */
void Program::add(const DataSeg& d)
{
    this->data_segments.push_back(d);
}

/*
 * getInstr()
 * Return the instruction at index idx
 */
Instr& Program::getInstr(const unsigned int idx) 
{
    if(idx < this->instructions.size())
        return this->instructions[idx];

    return this->null_instr;
}

DataSeg& Program::getData(const unsigned int idx) 
{
    if(idx < this->data_segments.size())
        return this->data_segments[idx];

    return this->null_data;
}

void Program::writeMem(const uint32_t addr, const uint32_t val)
{
    Instr ins;
    ins.adr = addr;
    ins.ins = val;
    this->instructions.push_back(ins);
}

unsigned int Program::size(void) const
{
    return this->instructions.size();
}

unsigned int Program::numDataSeg(void) const
{
    return this->data_segments.size();
}

unsigned int Program::dataSize(void) const
{
    unsigned int size = 0;
    for(unsigned int idx = 0; idx < this->data_segments.size(); ++idx)
    {
        size += this->data_segments[idx].data.size();
    }

    return size;
}


std::string Program::toString(void) const
{
    std::ostringstream oss;

    for(unsigned int i = 0; i < this->instructions.size(); ++i)
    {
        oss << "Instr " << std::setw(4) << std::dec << i << 
            " : " << this->instructions[i].toString() << std::endl;
    }

    return oss.str();
}


/*
 * numInstr()
 */
unsigned int Program::numInstrs(void) const
{
    return this->instructions.size();
}

/*
 * getInstr()
 */
Instr Program::getInstr(unsigned int idx) const
{
    if(idx < this->instructions.size())
        return Instr(this->instructions[idx]);
    return Instr();
}

/*
 * toVec()
 */
std::vector<uint8_t> Program::toVec(void)
{
    std::vector<uint8_t> prog;

    for(unsigned int i = 0; i < this->instructions.size(); ++i)
    {
        Instr cur_instr = this->instructions[i];
        prog.push_back((cur_instr.ins & 0xFF000000) >> 24);
        prog.push_back((cur_instr.ins & 0x00FF0000) >> 16);
        prog.push_back((cur_instr.ins & 0x0000FF00) >> 8);
        prog.push_back((cur_instr.ins & 0x000000FF) >> 0);
    }

    return prog;
}

/*
 * save()
 */
int Program::save(const std::string& filename) 
{
    uint32_t N;
    std::ofstream outfile;

    try{
        outfile.open(filename, std::ios::binary);
    }
    catch(std::ios_base::failure& e) {
        std::cerr << "[" << __func__ << "] " << e.what() << std::endl;
        return -1;
    }

    // TODO : figure out what a proper header should look like
    // write instruction segment
    N = (uint32_t) this->instructions.size();
    outfile.write(reinterpret_cast<char*>(&N), sizeof(uint32_t));

    outfile.write(
            reinterpret_cast<char*>(&this->instructions[0].adr),
            sizeof(uint32_t)
    );
    for(unsigned int idx = 0; idx < this->instructions.size(); ++idx)
    {
        if(this->verbose)
        {
            std::cout << "[" << __func__ << "] writing instr (" << std::dec
                << idx + 1 << "/" << this->instructions.size() << ") 0x" 
                << std::hex << std::setw(8) << std::setfill('0') 
                << unsigned(this->instructions[idx].ins) << std::endl;
        }
        outfile.write(
                reinterpret_cast<char*>(&this->instructions[idx].ins),
                sizeof(uint32_t)
        );
    }
    outfile.close();

    return 0;
}

/*
 * load()
 */
int Program::load(const std::string& filename) 
{
    std::ifstream infile;
    uint32_t num_records;
    uint32_t addr;

    this->instructions.clear();

    try {
        infile.open(filename, std::ios::binary);
    }
    catch(std::ios_base::failure& e) {
        std::cerr << "[" << __func__ << "] " << e.what() << std::endl;
    }

    // find how many records are in the file
    infile.read(reinterpret_cast<char*>(&num_records), sizeof(uint32_t));
    if(num_records == 0)
    {
        std::cerr << "[" << __func__ << "] no records in file " << 
            filename << std::endl;
    }

    // load the first address pointer 
    infile.read(reinterpret_cast<char*>(&addr), sizeof(uint32_t));

    Instr instr;
    for(unsigned int idx = 0; idx < num_records; ++idx)
    {
        infile.read(
                reinterpret_cast<char*>(&instr.ins), 
                sizeof(uint32_t)
        );
        instr.adr = addr;
        addr += 4;          // TODO : maybe make this a global const?
        this->instructions.push_back(instr);
    }
    infile.close();

    return 0;
}


void Program::setVerbose(bool v) 
{
    this->verbose = v;
}
bool Program::getVerbose(void) const
{
    return this->verbose;
}
