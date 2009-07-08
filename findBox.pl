#lattice = -0.50000000000000022204 + 0.86602540378443937374 I norm=1.000000
#sqrtLoxodromic = -1.73205080756887497273 + 0.99999999999999977796 I norm=4.000000
#parabolic = 0.49999999999999944489 + -0.00000000000000074940 I norm=0.250000
use POSIX qw(floor);
while (<>) {
	if (/(.*) = (.*) \+ (.*) I.*norm=(.*)/) {
		$real{$1} = $2;
		$imag{$1} = $3;
		$frac = 4*$4 - POSIX::floor(4*$4);
		$special = 'SPECIAL' if $frac < 0.000001 && $frac > -.000001;
		&printBox if ($1 eq 'parabolic');
	}
}

sub printBox {
	my $conv;
	if ($real{'parabolic'} < 0) {
		$conv .= "RP";
		$real{'parabolic'} = -$real{'parabolic'};
		$imag{'parabolic'} = -$imag{'parabolic'};
		$real{'lattice'} = -$real{'lattice'};
		$imag{'lattice'} = -$imag{'lattice'};
	}

	if ($imag{'parabolic'} < 0) {
		$conv .= "IP";
		$imag{'parabolic'} = -$imag{'parabolic'};
		$imag{'sqrtLoxodromic'} = -$imag{'sqrtLoxodromic'};
		$imag{'lattice'} = -$imag{'lattice'};
	}

	if ($imag{'lattice'} < 0) {
		$conv .= "IL";
		$imag{'lattice'} = -$imag{'lattice'};
		$real{'lattice'} = -$real{'lattice'};
	}

	if ($imag{'sqrtLoxodromic'} < 0) {
		$conv .= "IS";
		$imag{'sqrtLoxodromic'} = -$imag{'sqrtLoxodromic'};
		$real{'sqrtLoxodromic'} = -$real{'sqrtLoxodromic'};
	}
	my @coord;
	$coord[0] = $imag{'lattice'}        / (4 * 2**(-0.0/6.0));
	$coord[1] = $imag{'sqrtLoxodromic'} / (4 * 2**(-1.0/6.0));
	$coord[2] = $imag{'parabolic'}      / (4 * 2**(-2.0/6.0));
	$coord[3] = $real{'lattice'}        / (4 * 2**(-3.0/6.0));
	$coord[4] = $real{'sqrtLoxodromic'} / (4 * 2**(-4.0/6.0));
	$coord[5] = $real{'parabolic'}      / (4 * 2**(-5.0/6.0));
	my $box;
	for ($i = 0; $i < 120; ++$i) {
		$n = $i % 6;
		if ($coord[$n] >= 0) {
			$box .= "1";
			$coord[$n] = 2*$coord[$n] - 1;
		} else {
			$box .= "0";
			$coord[$n] = 2*$coord[$n] + 1;
		}
	}
	$area = ($real{'sqrtLoxodromic'}*$real{'sqrtLoxodromic'}
	       + $imag{'sqrtLoxodromic'}*$imag{'sqrtLoxodromic'}) * $imag{'lattice'};
	print "$box $area $special $conv\n";
	$special = '';
}
