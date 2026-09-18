import Base
import ../cachet.bend as Cachet

def main() -> IO(Unit):
  do IO<Unit>:
    +now : U32 <- Cachet.Clock.unix_now()
    IO.print(U32.show(now))
