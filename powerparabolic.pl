# perl script to search for possible impossible identities.

while ($word = <>) {
	chomp $word;
	@possiblePowers = &getPossiblePowers($word);
	foreach $line (@possiblePowers) {
		print $line, "\n";
	}
}

sub getPossiblePowers
{
	local ($word) = @_;
	local @x = ();
	local @y = ();
	local $m = 0;
	local $n = 0;
	local $i = 0;
	local $gs = '';
	local $ch = '';
	for ($i = 0; $i < length $word; ++$i) {
		$ch = substr $word, $i, 1;
		if ($ch eq 'g' || $ch eq 'G') {
			push(@x, $m);
			push(@y, $n);
			$gs .= $ch;
			$m = $n = 0;
		} elsif ($ch eq 'm') {
			++$m;
		} elsif ($ch eq 'M') {
			--$m;
		} elsif ($ch eq 'n') {
			++$n;
		} elsif ($ch eq 'N') {
			--$n;
		}
	}
	$x[0] += $m;
	$y[0] += $n;
	

	local ($numG, $n, $j, $base, $k, $numDiscrepancies, $discPos, $offset, $power, $sub, $subID) = ();
	local %found = ();
	$numG = length $gs;
	local @result = ();
N:	for ($n = 1; $n+$n <= $numG; ++$n) {
		# only consider divisors of $numG
		next if $numG % $n > 0;

		# first, check to make sure g has the correct sign
		for ($j = 1; $j*$n < $numG; ++$j) {
			next N if substr($gs, 0, $n) ne substr($gs, $n*$j, $n);
		}

		for ($base = $n; $base <= 2*$n && $base < $numG; $base += $n) {
			$numDiscrepancies = 0;
			$discPos = 0;
			for ($j = 0; $j < $numG; $j += $n) {
				for ($k = 0; $k < $n; ++$k) {
					next if $j == 0 && $k == 0;
					if ($x[$k + $j] != $x[$k + $base] || $y[$k + $j] != $y[$k + $base]) {
						++$numDiscrepancies;
						$discPos = $j + $k;
					}
				}
			}
			if ($numDiscrepancies < 2) {
				$offset = $base/$n;
				$power = $numG / $n;
				$sub = '';
				for ($k = 0; $k < $n; ++$k) {
					if ($x[$base+$k] > 0) {
						$sub .= 'm' x $x[$base+$k];
					} elsif ($x[$base+$k] < 0) {
						$sub .= 'M' x -$x[$base+$k];
					}
					if ($y[$base+$k] > 0) {
						$sub .= 'n' x $y[$base+$k];
					} elsif ($y[$base+$k] < 0) {
						$sub .= 'N' x -$y[$base+$k];
					}
					$sub .= substr($gs, $base+$k, 1);
				}
#				print "HERE word=$word n=$n base=$base sub=$sub\n";
				local $subIDAllowed = ($n > 1) ? 1 : 0;
				local $subLine = '';
				if ($subIDAllowed) {
#					print "sub=$sub\n";
					local @subLines = &getPossiblePowers($sub);
#					print "SUB($sub)=", $#subLines, "\n";
					foreach (@subLines)  {
						my @f = split(/ /);
						if (!$f[3] || ($f[4] == 0 && $f[5] == 0)) {
							$subLine = "{$_}";
							$subIDAllowed = 0;
						}
					}
				}
				local $matchRequired = ($discPos == 0) ? 0 : 1;
				if ($subIDAllowed && $matchRequired) {
					local $discBasePos = $base + ($discPos % $n);
					local $diffX = $x[$discBasePos] - $x[$discPos];
					local $diffY = $y[$discBasePos] - $y[$discPox];
					if ($diffX % $power != 0 || $diffy % $power != 0) {
#						$matchRequired = 0;
					}
				}
				local $matchX = $x[$base]-$x[0];
				local $matchY = $y[$base]-$y[0];
#				print "THERE word=$word n=$n base=$base sub=$sub\n";
					
				$line = join(" ", 'PossiblePower', $word, $subIDAllowed, $matchRequired, $matchX, $matchY, "$sub^$power ~ $discPos n=$n base=$base", $subLine);
				if ($subIDAllowed == 0 && $matchRequired == 0) {
					return ($line);
				} else {
					push(@result, $line);
				}
			}
		}
	}
#	print "returning($word)=", $#result, "\n";
	@result;
}
