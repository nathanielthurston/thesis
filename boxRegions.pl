sub coords {
	my ($box) = @_;
	my @coord;
	for ($i = 0; $i < length $box; ++$i) {
		$coord[$i % 6] .= substr $box, $i, 1;
	}
	@coord;
}

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

sub neighbors
{
	my ($box) = @_;
	my @neighbors = ();
	for ($i = 0; $i < 6; ++$i) {
		my $plusBox = $box;
		for ($pos = (length $box) - $i - 1; $pos >= 0; $pos -= 6) {
			if (substr($box, $pos, 1) eq '0') {
				substr($plusBox, $pos, 1) = '1';
				push(@neighbors, $plusBox);
				last;
			} else {
				substr($plusBox, $pos, 1) = '0';
			}
		}

		my $minusBox = $box;
		for ($pos = (length $box) - $i - 1; $pos >= 0; $pos -= 6) {
			if (substr($box, $pos, 1) eq '1') {
				substr($minusBox, $pos, 1) = '0';
				push(@neighbors, $minusBox);
				last;
			} else {
				substr($minusBox, $pos, 1) = '1';
			}
		}
	}
	@neighbors;
}

sub representative {
	my ($box) = @_;
	die "bad box" unless defined $boxStatus{$box};
	my ($box) = @_;
	if ($box eq $boxStatus{$box}) {
		return $box;
	} else {
		my $rep = &representative($boxStatus{$box});
#		print "representative($box) = $rep\n";
		$boxStatus{$box} = $rep;
		return $rep;
	}
}
		
sub checkNeighbors
{
	my ($box, $parent) = @_;
	my $doSplit = 0;

#	print "checkNeighbors($box, $parent)\n";
	my $rep = &representative($parent);
	foreach $adj (&neighbors($box)) {
		if (defined $boxStatus{$adj}) {
			if ($boxStatus{$adj} eq 'S') {
				$doSplit = 1;
			} else {
				my $other = &representative($adj);
				if ($other ne $rep) {
#					print "joining $other with $rep\n";
					$boxStatus{$other} = $rep;
				}
			}
		}
	}
	if ($doSplit) {
#		print "split($box)\n";
		&checkNeighbors($box . '0', $parent);
		&checkNeighbors($box . '1', $parent);
	}
}


while (<>) {
	if (/^([01]*) ([vms0-9\/\(\)]*) .*(variety|HOLE)/) {
		$name{$1} = $2;
	} else {
		chomp;
		push(@boxes, $_)
	}
}

sub nameRepresentative {
	my ($box, $name) = @_;
	for ($i = (length $box) - 1; $i >= 0; --$i) {
		my $sb = substr($box, 0, $i);
		if (defined $boxStatus{$sb}) {
			if ($boxStatus{$sb} ne 'S') {
				my $rep = &representative($sb);
				$names{$rep} .= "$name ";
			}
			last;
		}
	}
}
	
foreach $box (@boxes) {
	$boxStatus{$box} = $box;
#		print "processing $box\n";
	for ($i = length($box)-1; $i > 0; --$i) {
		my $sb = substr($box, 0, $i);
		last if $boxStatus{$sb} eq 'S';
		die "split/variety box" if defined $boxStatus{$sb};
		$boxStatus{$sb} = 'S';
	}
}

foreach $box (@boxes) {
	&checkNeighbors($box, $box);
}

$log2 = log(2);
foreach $box (@boxes) {
	my $rep = &representative($box);
	++$boxCount{$rep};
	$boxArea{$rep} += exp(-(length $box) * $log2);
	$parent{$rep} = $rep unless defined $parent{$rep};
	while ($box !~ /^$parent{$rep}/) {
		chop $parent{$rep};
	}

	my @coords = &coords($box);
	if (defined $bounds{$rep}) {
		for ($i = 0; $i < 6; ++$i) {
			${$bounds{$rep}}[$i] = $coords[$i] if ($coords[$i] lt ${$bounds{$rep}}[$i]);
			${$bounds{$rep}}[$i+6] = $coords[$i] if ($coords[$i] gt ${$bounds{$rep}}[$i+6]);
		}
	} else {
		my @foo = @coords;
		push(@foo, @coords);
		$bounds{$rep} = \@foo;
	}
}

foreach (sort keys %name) {
	&nameRepresentative($_, $name{$_});
}
foreach $rep (sort keys %boxCount) {
	$ba = sprintf("%e", $boxArea{$rep});
	for ($i = 0; $i < 6; ++$i) {
		$b0 = ${$bounds{$rep}}[$i];
		$b1 = ${$bounds{$rep}}[$i+6];
		$s = `echo "20k 2o2i .$b1 .$b0 -p" | dc`;
		chomp $s;
		$s =~ s/\./+/;
		$s =~ s/0*$//;
		$s = '' if $s eq '+';
		$size[$i] = $s;
	}
	my $bounds = "${$bounds{$rep}}[0]$size[0],${$bounds{$rep}}[1]$size[1],${$bounds{$rep}}[2]$size[2],${$bounds{$rep}}[3]$size[3],${$bounds{$rep}}[4]$size[4],${$bounds{$rep}}[5]$size[5]";
	print "\t$rep: $boxCount{$rep} $ba $parent{$rep} $bounds $names{$rep}\n";
#	foreach $box (@boxes) {
#		print "$box\n" if $boxStatus{$box} eq $rep;
#	}
}
