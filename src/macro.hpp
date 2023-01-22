/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-20
 * Updated: 2023-01-20
 *===============================*/

#ifndef DS_MACRO_HPP
#define DS_MACRO_HPP

#include "./expected/def.hpp"

#define def_err_vals __FILE__, __FUNCTION__, __LINE__

/**
 * If the function returns an error, then the function caller will immmediately
 * return the optional<error> value
 *
 * else the function resumes
 */
#define try_opt(opt)                                                           \
  if (auto err = opt) {                                                        \
    err->push_back_location(def_err_vals);                                     \
    return std::move(err);                                                     \
  }

#define try_opt_unexp(opt)                                                     \
  if (auto err = opt) {                                                        \
    err->push_back_location(def_err_vals);                                     \
    return ds::unexpected(std::move(*err));                                    \
  }

#define try_exp(exp)                                                           \
  ({                                                                           \
    auto res = exp;                                                            \
    if (!res) {                                                                \
      res.error().push_back_location(def_err_vals);                            \
      return std::move(res);                                                   \
    }                                                                          \
    std::move(*res);                                                           \
  });

#define try_exp_dis(exp)                                                       \
  {                                                                            \
    auto res = exp;                                                            \
    if (!res) {                                                                \
      res.error().push_back_location(def_err_vals);                            \
      return std::move(res);                                                   \
    }                                                                          \
  };

#define try_exp_err(exp)                                                       \
  ({                                                                           \
    auto res = exp;                                                            \
    if (!res) {                                                                \
      exp.error().push_back_location(def_err_vals);                            \
      return std::move(res.error());                                           \
    }                                                                          \
    std::move(*res);                                                           \
  });

#define try_exp_unexp(exp)                                                     \
  ({                                                                           \
    auto res = exp;                                                            \
    if (!res) {                                                                \
      res.error().push_back_location(def_err_vals);                            \
      return ds::unexpected(std::move(res.error()));                           \
    }                                                                          \
    *res;                                                                      \
  });

#endif
