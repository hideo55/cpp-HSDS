#ifndef HSDS_EXCEPTION_H_
#define HSDS_EXCEPTION_H_

#include <exception>
#include <stdexcept>
#include <string>
#ifndef _MSC_VER
 #include <stdint.h>
#endif // _MSC_VER

#ifndef HSDS_EXCEPTION_IF
#define HSDS_EXCEPTION_IF(cond__, message__) if(cond__){ \
    throw hsds::Exception(message__, __FILE__, __func__, __LINE__); \
}
#endif

namespace hsds {
/**
 * @class Exception
 * @brief Exception class for HSDS libraries
 */
class Exception: public std::exception {
public:

    /**
     * @brief Constructor
     * @param message[in] Description message of the exception
     * @param filename[in] Name of file that exception occurred.
     * @param func[in] Name of function that exception occurred.
     * @param line[in] Number of line that exception occurred.
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
     * @return Exception message
     */
    const char* what() const throw () {
        return message_.c_str();
    }

    /**
     * @brief Returns file name of the exception occurred.
     * @return File name
     */
    const char* getFileName() const {
        return filename_;
    }

    /**
     * @brief Returns function name of the exception occurred.
     * @return Function name
     */
    const char* getFunctionName() const {
        return func_;
    }

    /**
     * @brief Returns line number of the exception occurred.
     * @return Line number
     */
    const int getLineNumber() const {
        return line_;
    }

private:
    std::string message_;
    const char* filename_;
    const char* func_;
    uint32_t line_;
};
}

#endif /* HSDS_EXCEPTION_H_ */
