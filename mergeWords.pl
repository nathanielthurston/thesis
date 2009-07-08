while (<>) {
	chomp;
	($n, $w) = split;
	$count{$w} += $n;
}

foreach $word (sort keys %count) {
	print "$count{$word} $word\n";
}
