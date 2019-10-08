//
// Created by along on 19-4-6.
//

#ifndef REFLECTION_H
#define REFLECTION_H

#include <map>
#include <string>
#include <utility>
#include <functional>

namespace along {

    template<typename T, typename T_Ref>
    class Reflection {
    public:
        static Reflection *Instance() {
            static auto *instance = new Reflection();
            return instance;
        }

        static std::function<T_Ref(T *)> Ref(const std::string &name) {
            auto iter = Instance()->Dev_map.find(name);
            if (iter == Instance()->Dev_map.end()) {
                return [](T *context) -> T_Ref {
                    return T_Ref();
                };
            }
            return iter->second;
        }

        static void Register(const std::string &name, std::function<T_Ref(T *)> ref) {
            Instance()->Dev_map[name] = std::move(ref);
        }

    private:
        Reflection() = default;

        std::map<std::string, std::function<T_Ref(T *)>> Dev_map;
    };

    template<typename T, typename T_Ref>
    class RefRegister {
    public:
        RefRegister(const std::string &name, std::function<T_Ref(T *)> func) {
            Reflection<T, T_Ref>::Register(name, std::move(func));
        }
    };
}

#define REFLECTION(class_name, ref) along::Reflection<class_name, ref>

#define REGISTER(class_name, ref, func_name, reg_name, ...)                                           \
    along::RefRegister<class_name, ref> gRefRegister##class_name##func_name##reg_name(#reg_name,     \
    [](class_name *context) -> ref {                                                        \
        return std::bind(&class_name::func_name, context, __VA_ARGS__);                     \
    });

#define REGISTER_NONE(class_name, ref, func_name, reg_name)                                           \
    along::RefRegister<class_name, ref> gRefRegister##class_name##func_name##reg_name(#reg_name,     \
    [](class_name *context) -> ref {                                                        \
        return std::bind(&class_name::func_name, context);                     \
    });

#define REGISTER1st(class_name, ref, func_name) \
    REGISTER(class_name, ref, func_name, func_name, std::placeholders::_1)

#define REGISTER2nd(class_name, ref, func_name) \
    REGISTER(class_name, ref, func_name,func_name, std::placeholders::_1,std::placeholders::_2)

#endif //REFLECTION_H
