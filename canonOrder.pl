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
while ($line = <>) {
        chomp $line;
		($sign, $word) = split(/ /, $line);
		if ($word eq '') {
			$word = $sign;
			$sign = '';
		}
        @versions = &versions($word);
		$key = "$sign $versions[0]";
        ++$count{$key};
        print "$word $key $count{$key}\n";
}

