/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-20
 *===============================*/

#ifndef DS_MACRO_HPP
#define DS_MACRO_HPP

#include "ds-error/types.hpp"

#define def_err_vals __FILE__, __LINE__

#define BAD_ALLOC_OPT                                                          \
  ds::error { DS_ERROR_BAD_ALLOC }

#define BAD_ALLOC_EXP                                                          \
  ds::unexpected<ds::error> {                                                  \
    ds::error { DS_ERROR_BAD_ALLOC }                                           \
  }

#define OUT_OF_RANGE_OPT                                                       \
  ds::error { DS_ERROR_OUT_OF_RANGE }

#define OUT_OF_RANGE_EXP                                                       \
  ds::unexpected<ds::error> {                                                  \
    ds::error { DS_ERROR_OUT_OF_RANGE }                                        \
  }

#define EMPTY_OPT                                                              \
  ds::error { DS_ERROR_EMPTY }

#define EMPTY_EXP                                                              \
  ds::unexpected<ds::error> {                                                  \
    ds::error { DS_ERROR_EMPTY }                                               \
  }

#define FULL_OPT                                                               \
  ds::error { DS_ERROR_FULL }

#define FULL_EXP                                                               \
  ds::unexpected<ds::error> {                                                  \
    ds::error { DS_ERROR_FULL }                                                \
  }

#define NOT_FOUND_OPT                                                          \
  ds::error { DS_ERROR_NOT_FOUND }

#define NOT_FOUND_EXP                                                          \
  ds::unexpected<ds::error> {                                                  \
    ds::error { DS_ERROR_NOT_FOUND }                                           \
  }

#define SIZE_OPT                                                               \
  ds::error { DS_ERROR_SIZE }

#define SIZE_EXP                                                               \
  ds::unexpected<ds::error> {                                                  \
    ds::error { DS_ERROR_SIZE }                                                \
  }

#define ALREADY_SET_OPT                                                        \
  ds::error { DS_ERROR_ALREADY_SET }

#define ALREADY_SET_EXP                                                        \
  ds::unexpected<ds::error> {                                                  \
    ds::error { DS_ERROR_ALREADY_SET }                                         \
  }

#define NOT_IMPLEMENTED_OPT                                                    \
  ds::error { DS_ERROR_NOT_IMPLEMENTED, def_err_vals }

#define NOT_IMPLEMENTED_EXP                                                    \
  ds::unexpected<ds::error> {                                                  \
    ds::error { DS_ERROR_NOT_IMPLEMENTED, def_err_vals }                       \
  }

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
    return ds::unexpected<ds::error>{std::move(*err)};                         \
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
      res.error().push_back_location(def_err_vals);                            \
      return std::move(res.error());                                           \
    }                                                                          \
    std::move(*res);                                                           \
  });

#define try_exp_unexp(exp)                                                     \
  ({                                                                           \
    auto res = exp;                                                            \
    if (!res) {                                                                \
      res.error().push_back_location(def_err_vals);                            \
      return ds::unexpected<ds::error>{std::move(res.error())};                \
    }                                                                          \
    std::move(*res);                                                           \
  });

#endif
