#include "replay.h"

struct replay *compare_replays(struct replay *r1, struct replay *r2)
{
   int m1 = r1->mode;
   int m2 = r2->mode;
   int el1 = r1->ending_level;
   int sl1 = r1->starting_level;
   int el2 = r2->ending_level;
   int sl2 = r2->starting_level;
   int t1 = r1->time;
   int t2 = r2->time;

   if(m1 < m2) {
      return r1;
   } else if(m1 > m2) {
      return r2;
   } else if(m1 == m2) {
      if(sl1 < sl2)
         return r1;
      else if(sl1 > sl2)
         return r2;
      else {
         if(el1 - sl1 < el2 - sl2) {
            return r2;
         } else if(el1 - sl1 > el2 - sl2) {
            return r1;
         } else if(el1 - sl1 == el2 - sl2) {
            if(t1 < t2) {
               return r1;
            } else if(t1 > t2) {
               return r2;
            } else if(t1 == t2) {
               return r1;
            }
         }
      }
   }

   return r1;
}
