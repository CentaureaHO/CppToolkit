#include "lc_natural.h"
#include "lc_list.h"
#include "lc_tape.h"
#include "lc_combinators.h"
#include "lc_bool.h"
#include <iostream>
#include <functional>

using namespace Cele::LC;

static void printList(const List<Natural>& lst)
{
    List<Natural> curr = lst;
    while (!as_bool(IS_NIL<Natural>(curr)))
    {
        Natural val = HEAD<Natural>(curr);
        std::cout << as_uint(val) << " ";
        curr = TAIL<Natural>(curr);
    }
}

static void printTape(const Tape& tape)
{
    auto left_list   = FIRST(tape);
    auto current_val = FIRST(SECOND(tape));
    auto right_list  = SECOND(SECOND(tape));
    std::cout << "Left: [";
    printList(left_list);
    std::cout << "] ";
    std::cout << "Current: " << as_uint(current_val) << " ";
    std::cout << "Right: [";
    printList(right_list);
    std::cout << "]";
}

template <typename T>
inline const auto IF_THEN_ELSE_T = [](const Bool& condition) {
    return [condition](const std::function<T()>& then_thunk) {
        return [condition, then_thunk](const std::function<T()>& else_thunk) -> T {
            void* selected_thunk_ptr = condition((void*)&then_thunk)((void*)&else_thunk);
            return (*static_cast<std::function<T()>*>(selected_thunk_ptr))();
        };
    };
};

auto eqChar = [](char a, char b) -> Bool { return a == b ? TRUE : FALSE; };

int main()
{
    const char* bf_program_code =
        "++++++++[>+++++++++<-]>.+++++++++++++++++++++++++++++.+++++++..+++.>++++[>++++++++<-]>.>+++++[<+++++++++++>-]<"
        ".>++++[<++++++>-]<.+++.------.--------.>+++[>+++++++++++<-]>.>++++++++++. ";

    auto skip_rec_gen = [](auto self, int pos, uint32_t count, const char* program_code) -> int {
        if (count == 0) return pos;
        if (program_code[pos] == '\0') return pos;
        uint32_t new_count = (program_code[pos] == '[' ? count + 1 : (program_code[pos] == ']' ? count - 1 : count));
        return self(pos + 1, new_count, program_code);
    };

    FixPoint<decltype(skip_rec_gen)> skip_rec_fixed(std::move(skip_rec_gen));

    auto skip_rec = [skip_rec_fixed](int pos) {
        return [skip_rec_fixed](int pos, uint32_t count) {
            return [skip_rec_fixed, pos, count](
                       const char* program_code) -> int { return skip_rec_fixed(pos, count, program_code); };
        }(pos, 1);
    };

    auto skip = [skip_rec](int pc) {
        return [skip_rec, pc](const char* program_code) -> int { return skip_rec(pc + 1)(program_code); };
    };

    std::function<std::function<std::pair<int, Tape>(Tape)>(int)> eval;
    eval = [bf_program_code, &eval, skip](int pc) {
        return [pc, bf_program_code, &eval, skip](Tape tape) -> std::pair<int, Tape> {
            return IF_THEN_ELSE_T<std::pair<int, Tape>>(eqChar(bf_program_code[pc], '\0'))(
                [=]() { return std::make_pair(pc, tape); })([=]() {
                return IF_THEN_ELSE_T<std::pair<int, Tape>>(eqChar(bf_program_code[pc], ']'))(
                    [=]() { return std::make_pair(pc + 1, tape); })([=]() {
                    return IF_THEN_ELSE_T<std::pair<int, Tape>>(eqChar(bf_program_code[pc], '>'))(
                        [=]() { return eval(pc + 1)(RIGHT(tape)); })([=]() {
                        return IF_THEN_ELSE_T<std::pair<int, Tape>>(eqChar(bf_program_code[pc], '<'))(
                            [=]() { return eval(pc + 1)(LEFT(tape)); })([=]() {
                            return IF_THEN_ELSE_T<std::pair<int, Tape>>(eqChar(bf_program_code[pc], '+'))(
                                [=]() { return eval(pc + 1)(INC_TAPE(tape)); })([=]() {
                                return IF_THEN_ELSE_T<std::pair<int, Tape>>(eqChar(bf_program_code[pc], '-'))(
                                    [=]() { return eval(pc + 1)(DEC_TAPE(tape)); })([=]() {
                                    return IF_THEN_ELSE_T<std::pair<int, Tape>>(eqChar(bf_program_code[pc], '.'))(
                                        [=]() {
                                            std::cout << static_cast<char>(as_uint(CURRENT_VAL(tape)));
                                            return eval(pc + 1)(tape);
                                        })([=]() {
                                        return IF_THEN_ELSE_T<std::pair<int, Tape>>(eqChar(bf_program_code[pc], ','))(
                                            [=]() {
                                                char c;
                                                std::cin >> c;
                                                auto new_tape = WRITE_TAPE(tape)(as_lcnat(c));
                                                return eval(pc + 1)(new_tape);
                                            })([=]() {
                                            return IF_THEN_ELSE_T<std::pair<int, Tape>>(
                                                eqChar(bf_program_code[pc], '['))([=]() {
                                                Bool is_zero = (as_uint(CURRENT_VAL(tape)) == 0 ? TRUE : FALSE);
                                                return IF_THEN_ELSE_T<std::pair<int, Tape>>(is_zero)(
                                                    [=]() { return eval(skip(pc)(bf_program_code))(tape); })([=]() {
                                                    auto res = eval(pc + 1)(tape);
                                                    return eval(pc)(res.second);
                                                });
                                            })([=]() { return eval(pc + 1)(tape); });
                                        });
                                    });
                                });
                            });
                        });
                    });
                });
            });
        };
    };

    eval(0)(TAPE_NEW());
}
