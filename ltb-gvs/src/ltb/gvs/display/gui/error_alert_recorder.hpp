// ///////////////////////////////////////////////////////////////////////////////////////
// LTB Geometry Visualization Server
// Copyright (c) 2020 Logan Barnes - All Rights Reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/util/error.hpp"

// standard
#include <memory>

namespace ltb::gvs {

class ErrorAlert;

///
/// \brief Wraps an ErrorAlert and only exposes the error writing functions
///
class ErrorAlertRecorder {
public:
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    ErrorAlertRecorder(std::shared_ptr<ErrorAlert> const& popup);

    ///
    /// \return true if the error was successfully recorded, false otherwise
    ///
    [[nodiscard]] auto record_error(util::Error error) const -> bool;

    ///
    /// \return true if the warning was successfully recorded, false otherwise
    ///
    [[nodiscard]] auto record_warning(util::Error error) const -> bool;

private:
    std::weak_ptr<ErrorAlert> const error_alert_;
};

} // namespace ltb::gvs
