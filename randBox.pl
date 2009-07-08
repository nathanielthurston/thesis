$n = 1;
$n = $ARGV[0] if $#ARGV >= 0;
open(RANDOM, "</dev/random");
while ($n > 0) {
	read(RANDOM, $rbytes, 4);
	$rnum = unpack("L", $rbytes);
	$line = $rnum % 4737580;
	if ($count{$line} == 0) {
		++$count{$line};
		--$n;
	}
}
open(LINES, "cat decodeTree.out |");
while (<LINES>) {
	print if defined $count{$.}
}
