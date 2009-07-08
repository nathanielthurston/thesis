$box = "";
while (<>) {
	if (/X/) {
		push(@boxStack, $box . "1");
		$box = $box . "0";
	} else {
		print "$box $_";
		$box = pop @boxStack;
	}
}
