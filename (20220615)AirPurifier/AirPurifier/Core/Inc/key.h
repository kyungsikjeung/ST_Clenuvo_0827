#define ShortKey 0
#define LongKey 1

struct _Key{
  uint8_t Read;
  uint8_t Flag;
  uint8_t Step;
  uint8_t Buf;
  uint8_t DobuleCnt;
};



extern struct _Key Key;
extern void DoKey(void);