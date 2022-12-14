#ifndef _CHANNEL_H_INCLUDED
#define _CHANNEL_H_INCLUDED

#define CHANLEN 200
#define TOPICLEN 120
#define KEYLEN 23

#define MODE_INVITE  0x00000001
#define MODE_KEY     0x00000002
#define MODE_LIMIT   0x00000004
#define MODE_N       0x00000008
#define MODE_PRIVATE 0x00000010
#define MODE_SECRET  0x00000020
#define MODE_TOPIC   0x00000040
#define MODE_M       0x00000080

#define MODE_OP      0x00000001
#define MODE_VOICE   0x00000002
#define MODE_HALFOP  0x00000004
#define MODE_PEON    0x00000010

struct Channel {
  char name[CHANLEN + 1];
  dlink_list ops;
  dlink_list voices;
  dlink_list halfops;
  dlink_list peons;
  unsigned long mode;
  char key[KEYLEN + 1];
  char topic[TOPICLEN + 1];
  long limit;
  long ts;
};

#define T_PEON 0
#define T_OP 1
#define T_VOICE 2
#define T_HOP 3

extern dlink_list channels;

struct Channel *new_channel(char *);
void add_user_to_channel(struct Client *, struct Channel *, int);
struct Channel *find_channel(char *);
void remove_user_from_channel(struct Client *, struct Channel *);
void remove_from_all_channels(struct Client *);

#endif  
