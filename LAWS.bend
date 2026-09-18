import Base

# LAW: adding zero to any natural number yields the same number.
law add_zero:
  for x: Nat
  {Nat.add(x, 0n) == x : Nat}
