# description
```
it's a telegram bot that will motivate you and your friends to ecersise more
persistant. It will gamify gym/calistenic sessions.

main idea:
    every time you go exercise you launch a lobby with some name, non unique
    may be. So other friends can join your lobby/party or create their own ones.
    After a session member should write what he did: exercise repetitions load
    e.t.c.

    when you create a lobby it pings everybody to join you. It may delete
    previous messages, just not to spam. Lobby closes at 23:59 at the end of the
    day. If you've entered a lobby and did not write anythig you have one more
    day to insert your trainig. Elsewhere you're gonna lose your trainig mark.

    you ask bot for statistics: who made more trainigs, who did most pull ups
    or such stats.

    how to gamefy it more? I need to create a feeling that you're involved in
    some collective process.

    there will be reports every week and every month with stats that every user
    did. For example: did the user achieved his goals. Or leaderboard based on
    trainings and exercises.

    non mvp:
        achievements
```

# api
```
/trainig <lobby-name>
/join <lobby-name>
/done <workout-record>
/leaderboard
/config
/help
```

# lexer
```
// spaces are lexem separators

NAME       = LETTER, { LETTER | "_" | "-" }
LETTER     = [A-Za-zА-Яа-яЁё]
SEPARATOR  = "."
           | ","

INTEGER    = [1-9], { [0-9] }
           | 0

NUMBER     = INTEGER
           | INTEGER, [ SEPARATOR, FRACTION ]

FRACTION   = [0-9], { [0-9] }
LOAD_CMD   = "@"
RIR_CMD    = "&"

TRAIN_CMD       = "/training"
JOIN_CMD        = "/join"
DONE_CMD        = "/done"
HELP_CMD        = "/help"
CONFIG_CMD      = "/config"
LEADERBOARD_CMD = "/leaderboard"

WEEK  = "week"
MONTH = "month"
YEAR  = "year"
ALL   = "all"
```

# grammar
```
Message = TrainCmd,       LobbyName
        | JoinCmd,        LobbyName
        | DoneCmd,        WorkoutRecord
        | LeaderboardCmd, Timing
        | HelpCmd
        | ConfigCmd

WorkoutRecord  = ExerciseRecord, { ExerciseRecord }
ExerciseRecord = ExerciseName, ExerciseSet, { ExerciseSet }
ExerciseSet    = Count, [ ModifierPair ]
ModifierPair   = Load, [ Rir ]
               | Rir, [ Load ]

Rir          = RIR_CMD,  NUMBER
Load         = LOAD_CMD, NUMBER
Count        = INTEGER

LobbyName    = NAME
ExerciseName = NAME

TrainCmd     = TRAIN_CMD
JoinCmd      = JOIN_CMD
DoneCmd      = DONE_CMD
HelpCmd      = HELP_CMD

ConfigCmd    = CONFIG_CMD, NAME, NUMBER
             | CONFIG_CMD

LeaderboardCmd = LEADERBOARD_CMD
Timing         = WEEK
               | MONTH
               | YEAR
               | ALL
```
