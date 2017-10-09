#ifndef KWIVER_VITAL_KPF_PACKET_H_
#define KWIVER_VITAL_KPF_PACKET_H_

#include <vital/kpf/vital_kpf_export.h>
#include <utility>

namespace kwiver {
namespace vital {
namespace kpf {

  enum class VITAL_KPF_EXPORT packet_style
{
  INVALID,  // invalid, uninitialized
  ID,       // a numeric identifier (detection, track, event ID)
  TS,       // timestamp
  TSR,      // timestamp range
  LOC,      // location (2d / 3d)
  GEOM,     // bounding box
  POLY,     // polygon
  CONF,     // a confidence value
  EVENT,    // an event
  EVAL,     // an evaluation result
  ATTR,     // an attribute
  TAG,      // a tag
  KV        // a generic key/value pair
};

struct VITAL_KPF_EXPORT packet_header_t
{
  enum { NO_DOMAIN = -1 };

  packet_style style;
  int domain;
  packet_header_t(): style( packet_style::INVALID ), domain( NO_DOMAIN ) {}
  packet_header_t( packet_style s, int d ): style(s), domain(d) {}
};

VITAL_KPF_EXPORT auto packet_header_cmp = []( const packet_header_t& lhs, const packet_header_t& rhs )
{ return ( lhs.style == rhs.style )
  ? (lhs.domain < rhs.domain)
  : (lhs.style < rhs.style);
};

namespace canonical
{

struct VITAL_KPF_EXPORT bbox_t
{
  enum {IMAGE_COORDS = 0};
  double x1, y1, x2, y2;
  bbox_t( double a, double b, double c, double d): x1(a), y1(b), x2(c), y2(d) {}
};

struct VITAL_KPF_EXPORT id_t
{
  enum {DETECTION_ID=0, TRACK_ID, EVENT_ID };
  size_t d;
  id_t( size_t i ): d(i) {}
};

struct VITAL_KPF_EXPORT timestamp_t
{
  double d;
};

struct VITAL_KPF_EXPORT timestamp_range_t
{
  double start, stop;
};

} // ...canonical

union VITAL_KPF_EXPORT payload_t
{
  payload_t(): id(0) {}
  canonical::id_t id;
  canonical::timestamp_t timestamp;
  canonical::timestamp_range_t timestamp_range;
  canonical::bbox_t bbox;
  // ... use pointers for the polygon / events
  // pay special attention to cpctor / etc...
};

struct VITAL_KPF_EXPORT packet_t
{
  packet_header_t header;
  payload_t payload;
  packet_t(): header( packet_header_t() ) {}

};

} // ...kpf
} // ...vital
} // ...kwiver

#endif
