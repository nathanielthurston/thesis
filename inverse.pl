while (<>) {
	chomp;
	if (!defined $canon{$_}) {
		$orig = $_;
		$_ = reverse();
		s/([nN]+)([mM]+)/\2\1/g;
		s/g/#/g; s/G/g/g; s/#/G/g;
		s/m/#/g; s/M/m/g; s/#/M/g;
		s/n/#/g; s/N/n/g; s/#/N/g;
		if ($orig lt $_) {
			$canon{$orig} = $orig;
			$canon{$_} = $orig;
		} else {
			$canon{$orig} = $_;
			$canon{$_} = $_;
		}
	}
	print $canon{$_}, "\n";
}
