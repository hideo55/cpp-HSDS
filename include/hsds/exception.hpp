/**
 * @file exception.hpp
 * @brief Implementation of Exception class
 * @author Hideaki Ohno
 */

#if !defined(HSDS_EXCEPTION_H_)
#define HSDS_EXCEPTION_H_

#include <exception>
#include <stdexcept>
#include <iostream>
#include <string>
#if !defined(_MSC_VER)
 #include <stdint.h>
#endif // !defined(_MSC_VER)

#if !defined(HSDS_EXCEPTION_IF)
 #if defined(_MSC_VER)
#define HSDS_EXCEPTION_IF(cond__, message__) if(cond__){ \
    throw hsds::Exception(message__, __FILE__, __FUNCTION__, __LINE__); \
}
 #else // defined(_MSC_VER)
#define HSDS_EXCEPTION_IF(cond__, message__) if(cond__){ \
    throw hsds::Exception(message__, __FILE__, __func__, __LINE__); \
}
 #endif //defined(_MSC_VER)
#endif //!defined(HSDS_EXCEPTION_IF)

#if !defined(HSDS_DEBUG_IF)
  #if defined(_DEBUG)
    #define HSDS_DEBUG_IF(cond__, message__) HSDS_EXCEPTION_IF(cond__, message__)
  #else // defined(_DEBUG)
    #define HSDS_DEBUG_IF(cond__, message__)
  #endif //defined(_DEBUG)
#endif //!defined(HSDS_DEBUG_IF)

/**
 * @brief The namespace for Hide's Succinct Data Structures
 * @namespace hsds
 */
namespace hsds {

/**
 * @class Exception
 *
 * @brief Exception class for HSDS libraries
 */
class Exception: public std::exception {
public:

    /**
     * @brief Constructor
     *
     * @param[in] message Description message of the exception
     * @param[in] filename Name of file that exception occurred
     * @param[in] func Name of function that exception occurred
     * @param[in] line Number of line that exception occurred
     */
    Exception(const std::string& message, const char* filename, const char* func, uint32_t line) :
            message_(message), filename_(filename), func_(func), line_(line) {

    }

    /**
     * @brief Destructor
     */
    ~Exception() throw () {

    }

    /**
     * @brief Returns exception message as null terminated character sequence.
     *
     * @return Exception message
     */
    const char* what() const throw () {
        return message_.c_str();
    }

    /**
     * @brief Returns file name of the exception occurred.
     *
     * @return File name
     */
    const char* getFileName() const {
        return filename_;
    }

    /**
     * @brief Returns function name of the exception occurred.
     *
     * @return Function name
     */
    const char* getFunctionName() const {
        return func_;
    }

    /**
     * @brief Returns line number of the exception occurred.
     *
     * @return Line number
     */
    const int getLineNumber() const {
        return line_;
    }

    friend std::ostream& operator<<(std::ostream& os, hsds::Exception& e);

private:
    std::string message_;
    const char* filename_;
    const char* func_;
    uint32_t line_;
};

std::ostream& operator<<(std::ostream& os, hsds::Exception& e){
    os << e.what() << " at " << e.getFileName() << ":" << e.getLineNumber() << std::endl;
    return os;
}

}

#endif /* HSDS_EXCEPTION_H_ */
