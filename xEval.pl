while (<>) {
	if (/HOLE (.*) \((.*)\)/) {
		$box = $1;
		@words = split(/,/, $2);
		foreach $word (@words) {
			++$wordCount{$word};
		}
		push(@boxes, $box);
	}
}

open(EVAL, "|./Evaluate"); # | perl collateEval.pl");
foreach $box (@boxes) {
	foreach $word (sort keys %wordCount) {
		print EVAL "$box $word\n";
	}
}
