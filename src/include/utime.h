// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2006 Sage Weil <sage@newdream.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software 
 * Foundation.  See file COPYING.
 * 
 */

#ifndef CEPH_UTIME_H
#define CEPH_UTIME_H

#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

#include "include/types.h"
#include "common/strtol.h"


// --------
// utime_t

class utime_t {
public:
  struct {
    __u32 tv_sec, tv_nsec;
  } tv;

  friend class Clock;
 
 public:
  bool is_zero() const {
    return (tv.tv_sec == 0) && (tv.tv_nsec == 0);
  }
  void normalize() {
    if (tv.tv_nsec > 1000000000ul) {
      tv.tv_sec += tv.tv_nsec / (1000000000ul);
      tv.tv_nsec %= 1000000000ul;
    }
  }

  // cons
  utime_t() { tv.tv_sec = 0; tv.tv_nsec = 0; }
  utime_t(time_t s, int n) { tv.tv_sec = s; tv.tv_nsec = n; normalize(); }
  utime_t(const struct ceph_timespec &v) {
    decode_timeval(&v);
  }
  utime_t(const struct timeval &v) {
    set_from_timeval(&v);
  }
  utime_t(const struct timeval *v) {
    set_from_timeval(v);
  }
  void to_timespec(struct timespec *ts) const {
    ts->tv_sec = tv.tv_sec;
    ts->tv_nsec = tv.tv_nsec;
  }
  void set_from_double(double d) { 
    tv.tv_sec = (__u32)trunc(d);
    tv.tv_nsec = (__u32)((d - (double)tv.tv_sec) * (double)1000000000.0);
  }

  // accessors
  time_t        sec()  const { return tv.tv_sec; } 
  long          usec() const { return tv.tv_nsec/1000; }
  int           nsec() const { return tv.tv_nsec; }

  // ref accessors/modifiers
  __u32&         sec_ref()  { return tv.tv_sec; }
  __u32&         nsec_ref() { return tv.tv_nsec; }

  uint64_t to_nsec() const {
    return (uint64_t)tv.tv_nsec + (uint64_t)tv.tv_sec * 1000000000ull;
  }

  void copy_to_timeval(struct timeval *v) const {
    v->tv_sec = tv.tv_sec;
    v->tv_usec = tv.tv_nsec/1000;
  }
  void set_from_timeval(const struct timeval *v) {
    tv.tv_sec = v->tv_sec;
    tv.tv_nsec = v->tv_usec*1000;
  }

  void encode(bufferlist &bl) const {
    ::encode(tv.tv_sec, bl);
    ::encode(tv.tv_nsec, bl);
  }
  void decode(bufferlist::iterator &p) {
    ::decode(tv.tv_sec, p);
    ::decode(tv.tv_nsec, p);
  }

  void encode_timeval(struct ceph_timespec *t) const {
    t->tv_sec = tv.tv_sec;
    t->tv_nsec = tv.tv_nsec;
  }
  void decode_timeval(const struct ceph_timespec *t) {
    tv.tv_sec = t->tv_sec;
    tv.tv_nsec = t->tv_nsec;
  }

  utime_t round_to_minute() {
    struct tm bdt;
    time_t tt = sec();
    gmtime_r(&tt, &bdt);
    bdt.tm_sec = 0;
    tt = mktime(&bdt);
    return utime_t(tt, 0);
  }

  utime_t round_to_hour() {
    struct tm bdt;
    time_t tt = sec();
    gmtime_r(&tt, &bdt);
    bdt.tm_sec = 0;
    bdt.tm_min = 0;
    tt = mktime(&bdt);
    return utime_t(tt, 0);
  }

  // cast to double
  operator double() const {
    return (double)sec() + ((double)nsec() / 1000000000.0L);
  }
  operator ceph_timespec() const {
    ceph_timespec ts;
    ts.tv_sec = sec();
    ts.tv_nsec = nsec();
    return ts;
  }

  void sleep() const {
    struct timespec ts;
    to_timespec(&ts);
    nanosleep(&ts, NULL);
  }

  // output
  ostream& gmtime(ostream& out) const {
    out.setf(std::ios::right);
    char oldfill = out.fill();
    out.fill('0');
    if (sec() < ((time_t)(60*60*24*365*10))) {
      // raw seconds.  this looks like a relative time.
      out << (long)sec() << "." << std::setw(6) << usec();
    } else {
      // localtime.  this looks like an absolute time.
      //  aim for http://en.wikipedia.org/wiki/ISO_8601
      struct tm bdt;
      time_t tt = sec();
      gmtime_r(&tt, &bdt);
      out << std::setw(4) << (bdt.tm_year+1900)  // 2007 -> '07'
	  << '-' << std::setw(2) << (bdt.tm_mon+1)
	  << '-' << std::setw(2) << bdt.tm_mday
	  << ' '
	  << std::setw(2) << bdt.tm_hour
	  << ':' << std::setw(2) << bdt.tm_min
	  << ':' << std::setw(2) << bdt.tm_sec;
      out << "." << std::setw(6) << usec();
      out << "Z";
    }
    out.fill(oldfill);
    out.unsetf(std::ios::right);
    return out;
  }

  // output
  ostream& asctime(ostream& out) const {
    out.setf(std::ios::right);
    char oldfill = out.fill();
    out.fill('0');
    if (sec() < ((time_t)(60*60*24*365*10))) {
      // raw seconds.  this looks like a relative time.
      out << (long)sec() << "." << std::setw(6) << usec();
    } else {
      // localtime.  this looks like an absolute time.
      //  aim for http://en.wikipedia.org/wiki/ISO_8601
      struct tm bdt;
      time_t tt = sec();
      gmtime_r(&tt, &bdt);

      char buf[128];
      asctime_r(&bdt, buf);
      int len = strlen(buf);
      if (buf[len - 1] == '\n')
        buf[len - 1] = '\0';
      out << buf;
    }
    out.fill(oldfill);
    out.unsetf(std::ios::right);
    return out;
  }
  
  ostream& localtime(ostream& out) const {
    out.setf(std::ios::right);
    char oldfill = out.fill();
    out.fill('0');
    if (sec() < ((time_t)(60*60*24*365*10))) {
      // raw seconds.  this looks like a relative time.
      out << (long)sec() << "." << std::setw(6) << usec();
    } else {
      // localtime.  this looks like an absolute time.
      //  aim for http://en.wikipedia.org/wiki/ISO_8601
      struct tm bdt;
      time_t tt = sec();
      localtime_r(&tt, &bdt);
      out << std::setw(4) << (bdt.tm_year+1900)  // 2007 -> '07'
	  << '-' << std::setw(2) << (bdt.tm_mon+1)
	  << '-' << std::setw(2) << bdt.tm_mday
	  << ' '
	  << std::setw(2) << bdt.tm_hour
	  << ':' << std::setw(2) << bdt.tm_min
	  << ':' << std::setw(2) << bdt.tm_sec;
      out << "." << std::setw(6) << usec();
      //out << '_' << bdt.tm_zone;
    }
    out.fill(oldfill);
    out.unsetf(std::ios::right);
    return out;
  }

  int sprintf(char *out, int outlen) const {
    struct tm bdt;
    time_t tt = sec();
    localtime_r(&tt, &bdt);

    return snprintf(out, outlen,
		    "%04d-%02d-%02d %02d:%02d:%02d.%06ld",
		    bdt.tm_year + 1900, bdt.tm_mon + 1, bdt.tm_mday,
		    bdt.tm_hour, bdt.tm_min, bdt.tm_sec, usec());
  }

  static int32_t is_leap(int32_t year)
  {
    if(year % 400 == 0)
      return 1;
    if(year % 100 == 0)
      return 0;
    if(year % 4 == 0)
      return 1;
    return 0;
  }

  static int32_t days_from_0(int32_t year)
  {
    year--;
    return 365 * year + (year / 400) - (year/100) + (year / 4);
  }

  static int32_t days_from_1970(int32_t year)
  {
    static const int days_from_0_to_1970 = days_from_0(1970);
    return days_from_0(year) - days_from_0_to_1970;
  }

  static int32_t days_from_1jan(int32_t year,int32_t month,int32_t day)
  {
    static const int32_t days[2][12] =
    {
      { 0,31,59,90,120,151,181,212,243,273,304,334},
      { 0,31,60,91,121,152,182,213,244,274,305,335}
    };

    return days[is_leap(year)][month-1] + day - 1;
  }

  static time_t internal_timegm(tm const *t)
  {
    int year = t->tm_year + 1900;
    int month = t->tm_mon;
    if(month > 11)
    {
      year += month/12;
      month %= 12;
    }
    else if(month < 0)
    {
      int years_diff = (-month + 11)/12;
      year -= years_diff;
      month+=12 * years_diff;
    }
    month++;
    int day = t->tm_mday;
    int day_of_year = days_from_1jan(year,month,day);
    int days_since_epoch = days_from_1970(year) + day_of_year ;

    time_t seconds_in_day = 3600 * 24;
    time_t result = seconds_in_day * days_since_epoch + 3600 * t->tm_hour + 60 * t->tm_min + t->tm_sec;

    return result;
  }

  static int parse_date(const string& date, uint64_t *epoch, uint64_t *nsec,
                        string *out_date=NULL, string *out_time=NULL) {
    struct tm tm;
    memset(&tm, 0, sizeof(tm));

    if (nsec)
      *nsec = 0;

    const char *p = strptime(date.c_str(), "%Y-%m-%d", &tm);
    if (p) {
      if (*p == ' ') {
	p++;
	p = strptime(p, " %H:%M:%S", &tm);
	if (!p)
	  return -EINVAL;
        if (nsec && *p == '.') {
          ++p;
          unsigned i;
          char buf[10]; /* 9 digit + null termination */
          for (i = 0; (i < sizeof(buf) - 1) && isdigit(*p); ++i, ++p) {
            buf[i] = *p;
          }
          for (; i < sizeof(buf) - 1; ++i) {
            buf[i] = '0';
          }
          buf[i] = '\0';
          string err;
          *nsec = (uint64_t)strict_strtol(buf, 10, &err);
          if (!err.empty()) {
            return -EINVAL;
          }
        }
      }
    } else {
      return -EINVAL;
    }
    time_t t = internal_timegm(&tm);
    if (epoch)
      *epoch = (uint64_t)t;

    if (out_date) {
      char buf[32];
      strftime(buf, sizeof(buf), "%F", &tm);
      *out_date = buf;
    }
    if (out_time) {
      char buf[32];
      strftime(buf, sizeof(buf), "%T", &tm);
      *out_time = buf;
    }

    return 0;
  }
};
WRITE_CLASS_ENCODER(utime_t)


// arithmetic operators
inline utime_t operator+(const utime_t& l, const utime_t& r) {
  return utime_t( l.sec() + r.sec() + (l.nsec()+r.nsec())/1000000000L,
                  (l.nsec()+r.nsec())%1000000000L );
}
inline utime_t& operator+=(utime_t& l, const utime_t& r) {
  l.sec_ref() += r.sec() + (l.nsec()+r.nsec())/1000000000L;
  l.nsec_ref() += r.nsec();
  l.nsec_ref() %= 1000000000L;
  return l;
}
inline utime_t& operator+=(utime_t& l, double f) {
  double fs = trunc(f);
  double ns = (f - fs) * (double)1000000000.0;
  l.sec_ref() += (long)fs;
  l.nsec_ref() += (long)ns;
  l.normalize();
  return l;
}

inline utime_t operator-(const utime_t& l, const utime_t& r) {
  return utime_t( l.sec() - r.sec() - (l.nsec()<r.nsec() ? 1:0),
                  l.nsec() - r.nsec() + (l.nsec()<r.nsec() ? 1000000000:0) );
}
inline utime_t& operator-=(utime_t& l, const utime_t& r) {
  l.sec_ref() -= r.sec();
  if (l.nsec() >= r.nsec())
    l.nsec_ref() -= r.nsec();
  else {
    l.nsec_ref() += 1000000000L - r.nsec();
    l.sec_ref()--;
  }
  return l;
}
inline utime_t& operator-=(utime_t& l, double f) {
  double fs = trunc(f);
  double ns = (f - fs) * (double)1000000000.0;
  l.sec_ref() -= (long)fs;
  long nsl = (long)ns;
  if (nsl) {
    l.sec_ref()--;
    l.nsec_ref() = 1000000000L + l.nsec_ref() - nsl;
  }
  l.normalize();
  return l;
}


// comparators
inline bool operator>(const utime_t& a, const utime_t& b)
{
  return (a.sec() > b.sec()) || (a.sec() == b.sec() && a.nsec() > b.nsec());
}
inline bool operator<=(const utime_t& a, const utime_t& b)
{
  return !(operator>(a, b));
}
inline bool operator<(const utime_t& a, const utime_t& b)
{
  return (a.sec() < b.sec()) || (a.sec() == b.sec() && a.nsec() < b.nsec());
}
inline bool operator>=(const utime_t& a, const utime_t& b)
{
  return !(operator<(a, b));
}

inline bool operator==(const utime_t& a, const utime_t& b)
{
  return a.sec() == b.sec() && a.nsec() == b.nsec();
}
inline bool operator!=(const utime_t& a, const utime_t& b)
{
  return a.sec() != b.sec() || a.nsec() != b.nsec();
}


// output

// ostream
inline std::ostream& operator<<(std::ostream& out, const utime_t& t)
{
  return t.localtime(out);
}

#endif
