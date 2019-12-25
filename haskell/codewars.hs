import Text.Printf (printf)

-- https://www.codewars.com/kata/sum-of-digits-slash-digital-root/train/haskell
digitalRoot :: Integral a => a -> a
digitalRoot x
  | x < 10    = x
  | otherwise = digitalRoot $ (mod x 10) + digitalRoot (div x 10)

-- https://www.codewars.com/kata/sum-of-the-first-nth-term-of-series/train/haskell
seriesSum :: Integer -> String
seriesSum n =
    let nth = (fromInteger n) :: Int
        series = take nth [1/(1+3*j) | j <- [0..]]
        s = (sum series) :: Float
     in
        printf "%.2f" s

