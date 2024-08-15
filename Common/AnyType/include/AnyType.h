#pragma once

#include <memory>
#include <stdexcept>

namespace Celesteria
{

#ifdef USE_RAW_POINTER
#    define PTR_TYPE AnyBase*
#    define MAKE_PTR(ptr) (ptr)
#    define DELETE_PTR(ptr) delete ptr
#    define CLONE_PTR(ptr) (ptr ? ptr->clone() : nullptr)
#    define ACCESS_PTR(ptr) (ptr)
#else
#    define PTR_TYPE std::shared_ptr<AnyBase>
#    define MAKE_PTR(ptr) std::shared_ptr<AnyBase>(ptr)
#    define DELETE_PTR(ptr)
#    define CLONE_PTR(ptr) (ptr ? ptr->clone() : nullptr)
#    define ACCESS_PTR(ptr) (ptr.get())
#endif

    /**
     * @brief AnyBase类
     *
     * AnyBase是一个抽象基类，用于支持多态存储任意类型的对象。
     * 它定义了一个克隆方法，用于生成派生对象的副本。
     */
    class AnyBase
    {
      public:
        virtual ~AnyBase() = default;

        /**
         * @brief 克隆当前对象
         *
         * 该方法创建当前对象的副本并返回指向该副本的指针。
         *
         * @return PTR_TYPE 指向克隆对象的指针
         */
        virtual PTR_TYPE clone() const = 0;
    };

    /**
     * @brief AnyDerived模板类
     *
     * AnyDerived类用于存储具体的类型对象，并继承自AnyBase类。
     * 该类重写了clone方法，用于复制存储的具体类型对象。
     *
     * @tparam T 存储的具体类型
     */
    template <typename T>
    class AnyDerived : public AnyBase
    {
      public:
        /**
         * @brief 构造函数
         *
         * @param value 要存储的具体类型对象
         */
        AnyDerived(const T& value) : value(value) {}

        /**
         * @brief 克隆当前对象
         *
         * 该方法创建当前对象的副本并返回指向该副本的指针。
         *
         * @return PTR_TYPE 指向克隆对象的指针
         */
        PTR_TYPE clone() const override { return MAKE_PTR(new AnyDerived<T>(*this)); }

        T value;  ///< 存储的具体类型对象
    };

    /**
     * @brief AnyType类
     *
     * AnyType类用于存储任意类型的对象，支持类型安全的存取操作。
     * 该类内部使用智能指针或裸指针来管理存储的对象，具体取决于编译时的配置。
     */
    class AnyType
    {
      public:
        /**
         * @brief 默认构造函数
         *
         * 创建一个空的AnyType对象。
         */
        AnyType() = default;

        /**
         * @brief 构造函数
         *
         * 使用指定的具体类型对象构造AnyType对象。
         *
         * @tparam T 要存储的具体类型
         * @param value 要存储的具体类型对象
         */
        template <typename T>
        AnyType(const T& value) : ptr(MAKE_PTR(new AnyDerived<T>(value)))
        {}

        /**
         * @brief 复制构造函数
         *
         * 创建另一个AnyType对象的副本。
         *
         * @param other 要复制的AnyType对象
         */
        AnyType(const AnyType& other) : ptr(CLONE_PTR(other.ptr)) {}

        /**
         * @brief 复制赋值运算符
         *
         * 将另一个AnyType对象的内容复制到当前对象。
         *
         * @param other 要复制的AnyType对象
         * @return 当前AnyType对象的引用
         */
        AnyType& operator=(const AnyType& other)
        {
            if (this != &other)
            {
                DELETE_PTR(ptr);
                ptr = CLONE_PTR(other.ptr);
            }
            return *this;
        }

        /**
         * @brief 析构函数
         *
         * 销毁AnyType对象，释放内部存储的资源。
         */
        ~AnyType() { DELETE_PTR(ptr); }

        /**
         * @brief 类型转换
         *
         * 将存储的对象转换为指定类型并返回。
         * 如果类型不匹配，则抛出std::bad_cast异常。
         *
         * @tparam T 要转换的目标类型
         * @return 转换后的目标类型对象
         * @throws std::bad_cast 如果存储的类型与目标类型不匹配
         */
        template <typename T>
        T cast() const
        {
            if (auto derived = dynamic_cast<AnyDerived<T>*>(ACCESS_PTR(ptr))) { return derived->value; }
            else { throw std::bad_cast(); }
        }

        /**
         * @brief 引用类型转换
         *
         * 将存储的对象转换为指定类型的引用并返回。
         * 如果类型不匹配，则抛出std::bad_cast异常。
         *
         * @tparam T 要转换的目标类型
         * @return 转换后的目标类型对象的引用
         * @throws std::bad_cast 如果存储的类型与目标类型不匹配
         */
        template <typename T>
        T& cast()
        {
            if (auto derived = dynamic_cast<AnyDerived<T>*>(ACCESS_PTR(ptr))) { return derived->value; }
            else { throw std::bad_cast(); }
        }

      private:
        PTR_TYPE ptr = nullptr;  ///< 存储的对象指针
    };

#undef PTR_TYPE
#undef MAKE_PTR
#undef DELETE_PTR
#undef CLONE_PTR
#undef ACCESS_PTR

}  // namespace Celesteria