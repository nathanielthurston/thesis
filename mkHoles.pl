foreach $file (@ARGV) {
	$root = $file;
	$root =~ s/[^01]//g;
	open(TREE, "<$file");
	$box = "";
	@boxStack = ();
	while (<TREE>) {
		if (/X/) {
			push(@boxStack, $box . "1");
			$box = $box . "0";
		} else {
			if (/HOLE/) {
				die "root box $root is hole" if $box eq '';
				print "$root$box\n";
			}
			$box = pop @boxStack;
		}
	}
	while ($#boxStack >= 0) {
		print "$root$box\n";
		$box = pop @boxStack;
	}
	print "$root$box\n" if ($box ne '');
	close(TREE);
}
