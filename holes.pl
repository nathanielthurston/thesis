$lastbox = '';
while (<>) {
	chomp;
	($box, $word) = split(/ /);
	$box = "B" . $box;
	if ($lastbox eq '') {
		$firstbox = $box;
	} else {
		$nextbox = $lastbox;
		$nextbox =~ s/1+$//;
		++$nextbox;
		$nextbox .= "0" x ((length $box) - (length $nextbox));
		if ($nextbox ne $box) {
			print "missing $nextbox : $box\n";
		}
	}
	$lastbox = $box;
}

print "RANGE $firstbox : $lastbox\n";
