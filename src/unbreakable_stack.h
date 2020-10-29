/**
 * @file
 *
 * @brief Implementation of unbreakable stack, with c-style templates
 */

#ifndef ISP_DZ3_STACK_H
#define ISP_DZ3_STACK_H

#include <utility>
#include <cstring>
#include <cstdio>
#include <typeinfo>

/**
 * Enum that represents stack state
 */
enum unbr_stack_state {
    /**
     * Everything ok
     */
    OK,

    /**
     * Size is less than zero
     */
    WRONG_SIZE,

    /**
     * Capacity is less than zero
     */
    WRONG_CAPACITY,

    /**
     * Capacity is less than stack actual size
     */
    CAPACITY_LESS_THAN_SIZE,

    /**
     * Left canary guard is broken
     */
    ATTACK_LEFT,

    /**
     * Right canary guard is broken
     */
    ATTACK_RIGHT,

    /**
     * Left canary data guard is broken
     */
    DATA_ATTACK_LEFT,

    /**
     * Right canary data guard is broken
     */
    DATA_ATTACK_RIGHT,

    HASH_CHANGED,

    /**
     * Returned by top/pop if stack size is zero
     */
    STACK_EMPTY
};

const int resizeFactor = 2;

/**
 * Return enum stack state name
 */
static const char* enumStateName(const unbr_stack_state& state) {
    switch (state) {
        case OK:
            return "ok";
        case WRONG_SIZE:
            return "WRONG_SIZE";
        case WRONG_CAPACITY:
            return "WRONG_CAPACITY";
        case CAPACITY_LESS_THAN_SIZE:
            return "CAPACITY_LESS_THAN_SIZE";
        case STACK_EMPTY:
            return "STACK_EMPTY";
        case ATTACK_LEFT:
            return "ATTACK_LEFT";
        case ATTACK_RIGHT:
            return "ATTACK_RIGHT";
        case DATA_ATTACK_LEFT:
            return "DATA_ATTACK_LEFT";
        case DATA_ATTACK_RIGHT:
            return "DATA_ATTACK_RIGHT";
        case HASH_CHANGED:
            return "HASH_CHANGED";
    }

    return "UNKNOWN";
}

/**
 * Checks the stack state and dumps if stack is not ok
 */
#define validateStack()  {                    \
    unbr_stack_state valid = getStackState(); \
    if (valid != OK) {                        \
        dump();                               \
        return valid;                         \
    }                                         \
}

#define VAR_NAME(var) #var

#endif

#ifndef STACK_EL_TYPE
#define STACK_EL_TYPE int
#endif

#ifndef STACK_CANARY_LEN
#define STACK_CANARY_LEN 1
#endif

#define CONCAT(param1, param2) param1##_##param2
#define StackName(type) CONCAT(unbr_stack, type)

/**
 * Implementation of unbreakable stack, with c-style templates
 */
struct StackName(STACK_EL_TYPE) {
private:

    long leftCanaryGuard[STACK_CANARY_LEN]{};


    STACK_EL_TYPE* data = nullptr;

    long sizeInternal = 0;
    long capacity = 0;

#ifdef USE_HASH_CHECK
    std::size_t hash = 0;
#endif

    char* name;

    long rightCanaryGuard[STACK_CANARY_LEN]{};

    [[nodiscard]] unbr_stack_state getStackState() const {
        if (sizeInternal < 0) return WRONG_SIZE;
        if (capacity < 0) return WRONG_CAPACITY;
        if (capacity < sizeInternal) return CAPACITY_LESS_THAN_SIZE;
#ifdef USE_HASH_CHECK
        if (hash != computeHash()) return HASH_CHANGED;
#endif

        for (int i = 0; i < STACK_CANARY_LEN; ++i) {
            if (leftCanaryGuard[i] != 0xDEADBEEF) return ATTACK_LEFT;
            if (rightCanaryGuard[i] != 0xDEADBEEF) return ATTACK_RIGHT;

            if (data != nullptr) {
                for (size_t j = i * sizeof(STACK_EL_TYPE), until = (i + 1) * sizeof(STACK_EL_TYPE); j < until; ++j) {
                    if (((char*) data)[j] != (char) 0xA6) return DATA_ATTACK_LEFT;
                }

                for (size_t j = (capacity - 1 - i) * sizeof(STACK_EL_TYPE), until = (capacity - 2 - i) * sizeof(STACK_EL_TYPE); j > until; --j) {
                    if (((char*) data)[j] != (char) 0xA6) return DATA_ATTACK_RIGHT;
                }
            }
        }

        return OK;
    }

    bool resizeIfNeeded() {
        if (sizeInternal >= capacity - 2 * STACK_CANARY_LEN) {
            capacity = capacity > 0 ? resizeFactor * capacity : 10 + 2 * STACK_CANARY_LEN;
            auto* newData = new STACK_EL_TYPE[capacity];

            for (long i = 0; i < STACK_CANARY_LEN; ++i) {
                for (size_t j = i * sizeof(STACK_EL_TYPE), until = (i + 1) * sizeof(STACK_EL_TYPE); j < until; ++j) {
                    ((char*) newData)[j] = (char) 0xA6;
                }

                for (size_t j = (capacity - 1 - i) * sizeof(STACK_EL_TYPE), until = (capacity - 2 - i) * sizeof(STACK_EL_TYPE); j > until; --j) {
                    ((char*) newData)[j] = (char) 0xA6;
                }
            }

            for (long i = STACK_CANARY_LEN, until = sizeInternal + STACK_CANARY_LEN; i < until; ++i) {
                newData[i] = data[i];
            }

            delete[] data;
            data = newData;

            return true;
        }

        return false;
    }

    [[nodiscard]] size_t computeHash() const {
        size_t newHash = 0;

        newHash = capacity + (newHash << 6u) + (newHash << 16u) - newHash;
        newHash = sizeInternal + (newHash << 6u) + (newHash << 16u) - newHash;

        return newHash;
    }

public:

    explicit StackName(STACK_EL_TYPE)(const char* varName = "unnamed_stack") {
        for (int i = 0; i < STACK_CANARY_LEN; ++i) {
            leftCanaryGuard[i] = 0xDEADBEEF;
            rightCanaryGuard[i] = 0xDEADBEEF;
        }
        name = new char[80];
        strncpy(name, varName, 79);
        name[79] = '\0';
    }

    ~StackName(STACK_EL_TYPE)() {
        delete[] data;
        delete[] name;

        capacity = -1;
        sizeInternal = 0;
    }

    /**
     * Pushes given element to the stack
     *
     * @param elem Element to push
     * @return Stack state
     */
    unbr_stack_state push(STACK_EL_TYPE elem) {
        validateStack()

        resizeIfNeeded();
        data[STACK_CANARY_LEN + sizeInternal++] = elem;

#ifdef USE_HASH_CHECK
        hash = computeHash();
#endif

        validateStack()
        return OK;
    }

    /**
     * Pops top element from the stack
     *
     * @param elem Element reference to pop into
     * @return Stack state
     */
    unbr_stack_state pop(STACK_EL_TYPE& elem) {
        validateStack()
        if (empty()) return STACK_EMPTY;

        elem = data[--sizeInternal + STACK_CANARY_LEN];

#ifdef USE_HASH_CHECK
        hash = computeHash();
#endif

        validateStack()
        return OK;
    }

    /**
     * Returns the top element of the stack without popping it
     *
     * @param elem Element reference to write top element into
     * @return Stack state
     */
    unbr_stack_state top(STACK_EL_TYPE& elem) {
        validateStack()
        if (empty()) return STACK_EMPTY;

        elem = data[sizeInternal - 1 + STACK_CANARY_LEN];

#ifdef USE_HASH_CHECK
        hash = computeHash();
#endif

        validateStack()
        return OK;
    }

    /**
     * Returns whether this stack should be treated as empty
     *
     * @return True if stack is empty, false otherwise
     */
    [[nodiscard]] bool empty() const {
        return sizeInternal == 0;
    }

    /**
     * Returns size of the stack
     *
     * @return Size of the stack
     */
    [[nodiscard]] long size() const {
        return sizeInternal;
    }

    /**
     * Dumps all stack info to the file
     *
     * @param path Path to the dump file
     */
    void dump(const char* path) {
        FILE* logFile = fopen(path, "a");
        if (logFile == nullptr) return;

        unbr_stack_state state = getStackState();
        const char* stateStr = enumStateName(state);
        fprintf(logFile, "%s '%s' (%s) [%p] {\n\tcapacity: %ld,\n\tsize: %ld,\n\tdata [%p]: {\n",
                typeid(this).name(), name, stateStr, this, capacity - 2 * STACK_CANARY_LEN, sizeInternal, data);

#ifdef STACK_EL_TO_STRING
        if (state == OK) {
            for (long i = 0; i < capacity - 2 * STACK_CANARY_LEN; ++i) {
                if (i < sizeInternal) {
                    fprintf(logFile, "\t\t*data[%ld]: %s\n", i, STACK_EL_TO_STRING(data[i + STACK_CANARY_LEN]));
                } else {
                    fprintf(logFile, "\t\t data[%ld]: (POISON)\n", i);
                }
            }
        }
#endif
        fprintf(logFile, "\t}\n}\n\n");

        fclose(logFile);
    }

    /**
     * Dumps all stack info to default file (unbr_stack_<type>.txt)
     */
    void dump() {
        const char* className = typeid(this).name();

        std::size_t pathLen = strlen(className) + 5;
        char* path = new char[pathLen];
        strcpy(path, className);
        path[pathLen - 5] = '.';
        path[pathLen - 4] = 't';
        path[pathLen - 3] = 'x';
        path[pathLen - 2] = 't';
        path[pathLen - 1] = '\0';

        dump(path);

        delete[] path;
    }
};
