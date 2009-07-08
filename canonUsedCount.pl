sub versions
{
	my ($w) = @_;
	my $wi = reverse $w;
	$wi =~ y/a-zA-Z/A-Za-z/;
	my @v;
	for ($i = 0; $i < length $w; ++$i) {
		push(@v, substr($w, $i) . substr($w, 0, $i));
		push(@v, substr($wi, $i) . substr($wi, 0, $i));
	}
	sort @v;
}
while (<>) {
	chomp;
	($file, $word, $n) = split(/ /);
	@versions = &versions($word);
	$count{$versions[0]} += $n;
	$totalCount += $n;
}

foreach (sort keys %count) {
	$gPow = $_;
	$gPow =~ s/[mMnN]//g;
	$gPow =~ s/././g;
	print "$count{$_} $_ $gPow \n";
}
