#pragma once

#include "type/GenericField.hpp"
#include "value/FieldValue.hpp"
#include "value/GenericFieldValue.hpp"
#include "ValueImporter.hpp"
#include "Exception.hpp"

#include "structure_export.h"

namespace structure
{
namespace detail
{
/** Helper class for implementing new field types.
 *
 * CRTP = Curiously Reccuring Template Pattern.
 */
template <typename Derived, typename Base, typename _Storage>
class FieldCrtp : public Base
{
    // This prevents hard-to-understand compilation errors.
    static_assert(is_structure<Base>::value, "Base must be a Structure.");

private:
    using This = FieldCrtp<Derived, Base, _Storage>;
    using ThisValue = FieldValue<Derived>;

public:
    using Base::Base;

    using Storage = _Storage;

    std::unique_ptr<GenericFieldValue> with(const std::string &value) const override
    {
        return std::make_unique<ThisValue>(*static_cast<const Derived *>(this), value);
    }

    /** @returns A parsed value.
     *
     * @throws ParseError in case the value can't be parsed into the field's type.
     */
    static _Storage fromString(const std::string &input)
    {
        _Storage parsed;
        if (not convertTo(input, parsed)) {
            throw ParseError(input);
        }
        return parsed;
    }

    std::string getTypeName() const override { return Derived::typeToString(); }

private:
    std::unique_ptr<StructureValue> doBuild(ValueImporter &importer,
                                            const std::string &path) const override
    {
        return importer.import(*this, path);
    }
};
} // namespace detail
}
