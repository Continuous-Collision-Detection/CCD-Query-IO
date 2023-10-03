// Modified version of rational-gmp.cpp from CVC3
// (https://cs.nyu.edu/acsys/cvc3/releases/1.5/doc/rational-gmp_8cpp-source.html)
// by Sergey Berezin. Licensed under the following license:
// https://cs.nyu.edu/acsys/cvc3/releases/1.5/doc/LICENSE.html

#pragma once

#include <ccd_io/logger.hpp>

#include <gmp.h>
#include <cassert>
#include <iostream>
#include <cmath>

namespace ccd_io {

/// @brief Helper class that wraps GMP's mpq_t (rational) type
class Rational {
public:
    Rational()
    {
        mpq_init(value);
        mpq_set_d(value, 0);
    }

    Rational(double d)
    {
        assert(std::isfinite(d));
        mpq_init(value);
        mpq_set_d(value, d);
        canonicalize();
    }

    Rational(float d)
    {
        assert(std::isfinite(d));
        mpq_init(value);
        double ddouble = d; // convert (float)d to double
        mpq_set_d(value, ddouble);
        canonicalize();
    }

    Rational(int i)
    {
        mpq_init(value);
        mpq_set_si(value, i, 1U);
        canonicalize();
    }

    Rational(long i)
    {
        mpq_init(value);
        mpq_set_si(value, i, 1U);
        canonicalize();
    }

    Rational(const mpq_t& v_)
    {
        mpq_init(value);
        mpq_set(value, v_);
        // canonicalize();
    }

    Rational(const Rational& other)
    {
        mpq_init(value);
        mpq_set(value, other.value);
    }

    Rational(const std::string& numerator, const std::string& denominator)
        : Rational(numerator + "/" + denominator)
    {
    }

    Rational(const std::string& str)
    {
        mpq_init(value);
        // The return value is 0 if the entire string is a valid number.
        const int r = mpq_set_str(value, str.c_str(), 10);
        if (r != 0) {
            log_and_throw_error("Invalid rational: {}", str);
        }
    }

    ~Rational() { mpq_clear(value); }

    friend Rational operator-(const Rational& v)
    {
        Rational r_out;
        mpq_neg(r_out.value, v.value);
        return r_out;
    }

    friend Rational operator+(const Rational& x, const Rational& y)
    {
        Rational r_out;
        mpq_add(r_out.value, x.value, y.value);
        return r_out;
    }

    friend Rational operator-(const Rational& x, const Rational& y)
    {
        Rational r_out;
        mpq_sub(r_out.value, x.value, y.value);
        return r_out;
    }

    friend Rational operator*(const Rational& x, const Rational& y)
    {
        Rational r_out;
        mpq_mul(r_out.value, x.value, y.value);
        return r_out;
    }

    friend Rational operator/(const Rational& x, const Rational& y)
    {
        Rational r_out;
        mpq_div(r_out.value, x.value, y.value);
        return r_out;
    }

    Rational& operator=(const Rational& x)
    {
        if (this == &x)
            return *this;
        mpq_set(value, x.value);
        return *this;
    }

    Rational& operator=(const double x)
    {
        mpq_set_d(value, x);
        // canonicalize();
        return *this;
    }

    Rational& operator=(const float x)
    {
        double xd = x;
        mpq_set_d(value, xd);
        // canonicalize();
        return *this;
    }

    Rational& operator=(const int x)
    {
        mpq_set_si(value, x, 1U);
        // canonicalize();
        return *this;
    }

    Rational& operator=(const long x)
    {
        mpq_set_si(value, x, 1U);
        // canonicalize();
        return *this;
    }

    template <typename T> bool operator<(const T& r1)
    {
        if constexpr (std::is_same<T, Rational>::value)
            return mpq_cmp(value, r1.value) < 0;
        else
            return *this < Rational(r1);
    }

    template <typename T> bool operator>(const T& r1)
    {
        if constexpr (std::is_same<T, Rational>::value)
            return mpq_cmp(value, r1.value) > 0;
        else
            return *this > Rational(r1);
    }

    template <typename T> bool operator<=(const T& r1)
    {
        if constexpr (std::is_same<T, Rational>::value)
            return mpq_cmp(value, r1.value) <= 0;
        else
            return *this <= Rational(r1);
    }

    template <typename T> bool operator>=(const T& r1)
    {
        if constexpr (std::is_same<T, Rational>::value)
            return mpq_cmp(value, r1.value) >= 0;
        else
            return *this >= Rational(r1);
    }

    template <typename T> bool operator==(const T& r1)
    {
        if constexpr (std::is_same<T, Rational>::value)
            return mpq_equal(value, r1.value);
        else
            return *this == Rational(r1);
    }

    template <typename T> bool operator!=(const T& r1)
    {
        if constexpr (std::is_same<T, Rational>::value)
            return !mpq_equal(value, r1.value);
        else
            return *this != Rational(r1);
    }

    operator double() const { return mpq_get_d(value); }

    operator std::string() const
    {
        return std::string(mpq_get_str(NULL, 10, value));
    }

    friend std::ostream& operator<<(std::ostream& os, const Rational& r)
    {
        return (os << std::string(r));
    }

    void canonicalize() { mpq_canonicalize(value); }

    int sign() const { return mpq_sgn(value); }

    long long numerator() { return Integer::from_numerator(*this); }
    long long denominator() { return Integer::from_denominator(*this); }

    std::string numerator_str() { return Integer::from_numerator(*this); }
    std::string denominator_str() { return Integer::from_denominator(*this); }

    void print_numerator() { Integer::from_numerator(*this).print(); }
    void print_denominator() { Integer::from_denominator(*this).print(); }

    /// @brief The actual GMP rational value
    mpq_t value;

protected:
    /// @brief Helper class that wraps GMP's mpz_t (integer) type
    class Integer {
    public:
        Integer() { mpz_init(value); }

        ~Integer() { mpz_clear(value); }

        static Integer from_numerator(const Rational& r)
        {
            Integer i;
            mpq_get_num(i.value, r.value);
            return i;
        }

        static Integer from_denominator(const Rational& r)
        {
            Integer i;
            mpq_get_den(i.value, r.value);
            return i;
        }

        void print() { mpz_out_str(NULL, 10, value); }

        operator long long() const
        {
            assert(mpz_fits_slong_p(value));
            return mpz_get_si(value);
        }

        operator std::string() const
        {
            return std::string(mpz_get_str(NULL, 10, value));
        }

        /// @brief The actual GMP integer value
        mpz_t value;
    };
};

// typedef Eigen::Matrix<Rational, 3, 1, Eigen::ColMajor | Eigen::DontAlign>
//     Vector3r;
// typedef Eigen::Array<Rational, 3, 1, Eigen::ColMajor | Eigen::DontAlign>
//     Array3r;

} // namespace ccd_io
