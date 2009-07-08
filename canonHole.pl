sub versions
{
        my ($w) = @_;
        my $wi = reverse $w;
        $wi =~ y/a-zA-Z/A-Za-z/;
        $wi =~ s/([nN]+)([mM]+)/\2\1/g;
        my @v;
        for ($i = 0; $i < length $w; ++$i) {
                push(@v, substr($w, $i) . substr($w, 0, $i));
                push(@v, substr($wi, $i) . substr($wi, 0, $i));
        }
        sort @v;
}
while (<>) {
	if (/HOLE ([01]*) \((.*)\)/) {
		$box = $1;
		@words = split(/,/, $2);
		%canonWords = ();
		foreach $word (@words) {
			if (!defined $canonVersion{$word}) {
				my @v = &versions($word);
				$canonVersion{$word} = $v[0];
			}
			$canonWords{$canonVersion{$word}} = 1;
		}
		@canonWords = sort keys %canonWords;
		print "HOLE $box (" . join(",", @canonWords) . ")\n";
	} else {
		print;
	}
}
