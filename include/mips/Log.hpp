/*
 * LOG
 * Logging stuff
 *
 * Stefan Wong 2019
 */

#ifndef __SMIPS_LOG_HPP
#define __SMIPS_LOG_HPP

#ifdef DEBUG_MODE 

#define SMIPS_DEBUG(x) do { \
    {std::cerr << "[" << __func__ << "] (" << __FILE__ << ":" << __LINE__ << ")" << x << std::endl; }\
}while (0)

#else
#define SMIPS_DEBUG(x) do {} while(0)
#endif /*DEBUG_MODE*/

#define SMIPS_PRINT(x) do { \
    {std::cerr << "[" << __func__ << "] (" << __FILE__ << ":" << __LINE__ << ")" << x << std::endl; }\
}while (0)

#endif /*__SMIPS_LOG_HPP*/

