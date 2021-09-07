#include "sanitizer/ubsanitizer.h"

#include "device/cgastr.h"

extern "C" {

static void print_location(const SourceLocation& location)
{
    if (!location.filename()) {
        kout << "KUBSAN: in unknown file" << endl;
    } else {
        kout << "KUBSAN: at " << location.filename() << ", line " << location.line() << ", column: " << location.column() << endl;
    }
    kout.flush();
    /*dump_backtrace();
    if (g_ubsan_is_deadly) {
        dbgln("UB is configured to be deadly, halting the system.");
        Processor::halt();
    }*/
}

void __ubsan_handle_load_invalid_value(const InvalidValueData&, ValueHandle) __attribute__((used));
void __ubsan_handle_load_invalid_value(const InvalidValueData& data, ValueHandle)
{
    kout << "KUBSAN: load-invalid-value: " << data.type.name() << " (" << data.type.bit_width() << "-bit)" << endl;
    print_location(data.location);
}

void __ubsan_handle_nonnull_arg(const NonnullArgData&) __attribute__((used));
void __ubsan_handle_nonnull_arg(const NonnullArgData& data)
{
    kout << "KUBSAN: null pointer passed as argument " << data.argument_index << ", which is declared to never be null" << endl;
    print_location(data.location);
}

void __ubsan_handle_nullability_arg(const NonnullArgData&) __attribute__((used));
void __ubsan_handle_nullability_arg(const NonnullArgData& data)
{
    kout << "KUBSAN: null pointer passed as argument " << data.argument_index << ", which is declared to never be null" << endl;
    print_location(data.location);
}

void __ubsan_handle_nonnull_return_v1(const NonnullReturnData&, const SourceLocation&) __attribute__((used));
void __ubsan_handle_nonnull_return_v1(const NonnullReturnData&, const SourceLocation& location)
{
    kout << "KUBSAN: null pointer return from function declared to never return null" << endl;
    print_location(location);
}

void __ubsan_handle_nullability_return_v1(const NonnullReturnData& data, const SourceLocation& location) __attribute__((used));
void __ubsan_handle_nullability_return_v1(const NonnullReturnData&, const SourceLocation& location)
{
    kout << "KUBSAN: null pointer return from function declared to never return null" << endl;
    print_location(location);
}

void __ubsan_handle_vla_bound_not_positive(const VLABoundData&, ValueHandle) __attribute__((used));
void __ubsan_handle_vla_bound_not_positive(const VLABoundData& data, ValueHandle)
{
    kout << "KUBSAN: VLA bound not positive " << data.type.name() << " (" << data.type.bit_width() << "-bit)" << endl;
    print_location(data.location);
}

void __ubsan_handle_add_overflow(const OverflowData&, ValueHandle lhs, ValueHandle rhs) __attribute__((used));
void __ubsan_handle_add_overflow(const OverflowData& data, ValueHandle, ValueHandle)
{
    kout << "KUBSAN: addition overflow, " << data.type.name() << " (" << data.type.bit_width() << "-bit)" << endl;

    print_location(data.location);
}

void __ubsan_handle_sub_overflow(const OverflowData&, ValueHandle lhs, ValueHandle rhs) __attribute__((used));
void __ubsan_handle_sub_overflow(const OverflowData& data, ValueHandle, ValueHandle)
{
    kout << "KUBSAN: subtraction overflow, " << data.type.name() << " (" << data.type.bit_width() << "-bit)" << endl;

    print_location(data.location);
}

void __ubsan_handle_negate_overflow(const OverflowData&, ValueHandle) __attribute__((used));
void __ubsan_handle_negate_overflow(const OverflowData& data, ValueHandle)
{
    kout << "KUBSAN: negation overflow, " << data.type.name() << " (" << data.type.bit_width() << "-bit)" << endl;

    print_location(data.location);
}

void __ubsan_handle_mul_overflow(const OverflowData&, ValueHandle lhs, ValueHandle rhs) __attribute__((used));
void __ubsan_handle_mul_overflow(const OverflowData& data, ValueHandle, ValueHandle)
{
    kout << "KUBSAN: multiplication overflow, " << data.type.name() << " (" << data.type.bit_width() << "-bit)" << endl;
    print_location(data.location);
}

void __ubsan_handle_shift_out_of_bounds(const ShiftOutOfBoundsData&, ValueHandle lhs, ValueHandle rhs) __attribute__((used));
void __ubsan_handle_shift_out_of_bounds(const ShiftOutOfBoundsData& data, ValueHandle, ValueHandle)
{
    kout << "KUBSAN: shift out of bounds" << endl; //, {} ({}-bit) shifted by {} ({}-bit)", data.lhs_type.name(), data.lhs_type.bit_width(), data.rhs_type.name(), data.rhs_type.bit_width());
    print_location(data.location);
}

void __ubsan_handle_divrem_overflow(const OverflowData&, ValueHandle lhs, ValueHandle rhs) __attribute__((used));
void __ubsan_handle_divrem_overflow(const OverflowData& data, ValueHandle, ValueHandle)
{
    kout << "KUBSAN: divrem overflow, " << data.type.name() << " (" << data.type.bit_width() << "-bit)" << endl;
    print_location(data.location);
}

void __ubsan_handle_out_of_bounds(const OutOfBoundsData&, ValueHandle) __attribute__((used));
void __ubsan_handle_out_of_bounds(const OutOfBoundsData& data, ValueHandle)
{
    kout << "KUBSAN: out of bounds access into array" << endl; // of {} ({}-bit), index type {} ({}-bit)", data.array_type.name(), data.array_type.bit_width(), data.index_type.name(), data.index_type.bit_width());
    print_location(data.location);
}

void __ubsan_handle_type_mismatch_v1(const TypeMismatchData&, ValueHandle) __attribute__((used));
void __ubsan_handle_type_mismatch_v1(const TypeMismatchData& data, ValueHandle ptr)
{
    // constexpr StringView kinds[] = {
    //     "load of",
    //     "store to",
    //     "reference binding to",
    //     "member access within",
    //     "member call on",
    //     "constructor call on",
    //     "downcast of",
    //     "downcast of",
    //     "upcast of",
    //     "cast to virtual base of",
    //     "_Nonnull binding to",
    //     "dynamic operation on"
    // };

    // FlatPtr alignment = (FlatPtr)1 << data.log_alignment;
    // auto kind = kinds[data.type_check_kind];

    // if (!ptr) {
    //     kout << "KUBSAN: {} null pointer of type {}", kind, data.type.name());
    // } else if ((FlatPtr)ptr & (alignment - 1)) {
    //     kout << "KUBSAN: {} misaligned address {:p} of type {}", kind, ptr, data.type.name());
    // } else {
    //     kout << "KUBSAN: {} address {:p} with insufficient space for type {}", kind, ptr, data.type.name());
    // }
    kout << "KUBSAN: type_mismatch_v1" << endl;

    print_location(data.location);
}

void __ubsan_handle_alignment_assumption(const AlignmentAssumptionData&, ValueHandle, ValueHandle, ValueHandle) __attribute__((used));
void __ubsan_handle_alignment_assumption(const AlignmentAssumptionData& data, ValueHandle pointer, ValueHandle alignment, ValueHandle offset)
{
    // if (offset) {
    //     kout << 
    //         "KUBSAN: assumption of {:p} byte alignment (with offset of {:p} byte) for pointer {:p}"
    //         "of type {} failed",
    //         alignment, offset, pointer, data.type.name());
    // } else {
    //     kout << "KUBSAN: assumption of {:p} byte alignment for pointer {:p}"
    //           "of type {} failed",
    //         alignment, pointer, data.type.name());
    // }
    kout << "KUBSAN: assumption of " << alignment << " byte alignment" << endl;

    print_location(data.location);
}

void __ubsan_handle_builtin_unreachable(const UnreachableData&) __attribute__((used));
void __ubsan_handle_builtin_unreachable(const UnreachableData& data)
{
    kout << "KUBSAN: execution reached an unreachable program point" << endl;
    print_location(data.location);
}

void __ubsan_handle_missing_return(const UnreachableData&) __attribute__((used));
void __ubsan_handle_missing_return(const UnreachableData& data)
{
    kout << "KUBSAN: execution reached the end of a value-returning function without returning a value" << endl;
    print_location(data.location);
}

void __ubsan_handle_implicit_conversion(const ImplicitConversionData&, ValueHandle, ValueHandle) __attribute__((used));
void __ubsan_handle_implicit_conversion(const ImplicitConversionData& data, ValueHandle, ValueHandle)
{
    // const char* src_signed = data.from_type.is_signed() ? "" : "un";
    // const char* dst_signed = data.to_type.is_signed() ? "" : "un";
    // kout << "KUBSAN: implicit conversion from type {} ({}-bit, {}signed) to type {} ({}-bit, {}signed)",
    //     data.from_type.name(), data.from_type.bit_width(), src_signed, data.to_type.name(), data.to_type.bit_width(), dst_signed);
    kout << "KUBSAN: implicit conversion" << endl;
    print_location(data.location);
}

void __ubsan_handle_invalid_builtin(const InvalidBuiltinData) __attribute__((used));
void __ubsan_handle_invalid_builtin(const InvalidBuiltinData data)
{
    kout << "KUBSAN: passing invalid argument" << endl;
    print_location(data.location);
}

void __ubsan_handle_pointer_overflow(const PointerOverflowData&, ValueHandle, ValueHandle) __attribute__((used));
void __ubsan_handle_pointer_overflow(const PointerOverflowData& data, ValueHandle base, ValueHandle result)
{
    if (base == 0 && result == 0) {
        kout << "KUBSAN: applied zero offset to nullptr" << endl;
    } else if (base == 0 && result != 0) {
        kout << "KUBSAN: applied non-zero offset " << result << " to nullptr" << endl;
    } else if (base != 0 && result == 0) {
        kout << "KUBSAN: applying non-zero offset to non-null pointer " << base << " produced null pointer" << endl;
    } else {
        kout << "KUBSAN: addition of unsigned offset to " << base << " overflowed to " << result << endl;
    }
    print_location(data.location);
}
}