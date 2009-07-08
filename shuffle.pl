while ($line = <>) {
	chomp $line;
	for ($i = 0; $i < length $line; ++$i) {
		$coord[$i % 6] .= substr $line, $i, 1;
	}
	print join(" ", @coord, $line), "\n";
	@coord = ();
}
