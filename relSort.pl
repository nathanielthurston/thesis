open(MOMS, "<momGenWords");
@moms = <MOMS>;
close MOMS;
open(PARAMS, "<paramGenWords");
@params = <PARAMS>;
close PARAMS;

foreach (@moms) {
	chomp;
	++$moms{$_}
}
foreach (@params) {
	chomp;
	++$params{$_};
}
while (<>) {
	chomp;
	$w = $_;
	s/[mMnN]//g;
	$l = length $_;
	if (defined $moms{$w}) {
		$k = 'b';
	} elsif (defined $params{$w}) {
		$k = 'c';
	} else {
		$k = 'a';
	}
	print "$k $l $w\n";
}
