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
	if (/^([01]*) ([vms0-9\/\(\)]*) .*variety/) {
		$name{$1} = $2;
	} elsif (/variety (.*)\((.*)\)/) {
		$word = $1;
		$box = $2;

		if (!defined $canonVersion{$word}) {
			@v = &versions($word);
			$canonVersion{$word} = $v[0];
		}
		$word = $canonVersion{$word};
		if (!defined $boxes{$word}) {
			my @boxes = ();
			$boxes{$word} = \@boxes;
		}
		push(@{$boxes{$word}}, $box);
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
	
foreach $word (sort keys %boxes) {
	print "$word: $#{$boxes{$word}} boxes\n";
	%boxStatus = ();
	foreach $box (@{$boxes{$word}}) {
		$boxStatus{$box} = $box;
#		print "processing $box\n";
		for ($i = length($box)-1; $i > 0; --$i) {
			my $sb = substr($box, 0, $i);
			last if $boxStatus{$sb} eq 'S';
			die "split/variety box" if defined $boxStatus{$sb};
			$boxStatus{$sb} = 'S';
		}
	}

	foreach $box (@{$boxes{$word}}) {
		&checkNeighbors($box, $box);
	}

	%boxCount = ();
	foreach $box (@{$boxes{$word}}) {
		my $rep = &representative($box);
		++$boxCount{$rep};
	}

	foreach (sort keys %name) {
		&nameRepresentative($_, $name{$_});
	}
	foreach $rep (sort keys %boxCount) {
		print "\t$rep: $boxCount{$rep} $names{$rep}\n";
	}
}
