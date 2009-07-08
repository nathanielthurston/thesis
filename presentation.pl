while (<>) {
	chomp;
	if (/Generators: (.*)/) {
		@generators = split(/ /, $1);
	} elsif (/relation: (.*)/) {
		push(@relations, $1);
	} elsif (/g: (.*)/) {
		$g = $1;
	} elsif (/m: (.*)/) {
		$m = $1;
	} elsif (/n: (.*)/) {
		$n = $1;
		&process;
	}
}

sub inverse
{
	my ($word) = @_;
	$word = reverse $word;
	$word =~ y/a-zA-Z/A-Za-z/;
	$word;
}

sub reduce
{
	my ($w, @null) = @_;
REDUCED:	while (1) {
		my $ww = "$w$w";
		foreach $r (@null) {
			my $pos = index($ww, $r);
			if ($pos >= 0) {
				print "$w / $r [$pos]";
				if ($pos + length ($r) > length ($w)) {
#					print "offset = ", $pos + length ($r) - length ($w), " length = ", length ($w) - length ($r), "\n";
					$w = substr($w, $pos + length ($r) - length ($w), length ($w) - length ($r));
				} else {
					$w = substr($w, 0, $pos) . substr($w, $pos + length ($r));
				}
				print " = $w\n";
				next REDUCED;
			}
		}
		last;
	}
	$w;
}

sub process
{
	my (@null, $gen, $invGen, $w, $wInv, $i);
	foreach $gen (@generators) {
		$invGen = &inverse($gen);
		push(@null, "$gen$invGen", "$invGen$gen");
	}
REDUCED: while(1) {
		foreach $w (@relations) {
			my $wlc = $w;
			$wlc =~ y/A-Z/a-z/;
			foreach $gen (@generators) {
				if ($wlc =~ /$gen/ && $wlc !~ /$gen.*$gen/) {
					if ($w =~ /(.*)$gen(.*)/) {
						print join(" ", 'gens', @generators, 'rels', @relations, 'm', $m, 'n', $n, 'g', $g), "\n";
						$invSub = $2 . $1;
						$sub = &inverse($invSub);
						$invGen = &inverse($gen);
						@relations = grep(!/$w/ && (s/$gen/$sub/g || s/$invGen/$invSub/g || 1), @relations);
						$m =~ s/$gen/$sub/g;
						$n =~ s/$gen/$sub/g;
						$g =~ s/$gen/$sub/g;
						$m =~ s/$invGen/$invSub/g;
						$n =~ s/$invGen/$invSub/g;
						$g =~ s/$invGen/$invSub/g;
						@generators = grep(!/$gen/, @generators);
						print "reduced $w / $gen ($gen -> $sub, $invGen -> $invSub)\n";
						next REDUCED;
					}
				}
			}
		}
		last;
	}
	print join(" ", 'gens', @generators, 'rels', @relations, 'm', $m, 'n', $n, 'g', $g), "\n";
	foreach $w (@relations) {
		my $v;
		foreach $v (@relations) {
			my $wv;
			$wv = &reduce("$w$v", @null);
			if ($wv ne '') {
				push(@null, $wv);
			}
		}
		for ($i = 0; $i < length $w; ++$i) {
			my $rw = substr($w, $i) . substr($w, 0, $i);
			push(@null, $rw, inverse($rw));
		}
	}
	print "gens = ", join(" ", @generators), "; rels = ", join(" ", @relations), "\n";
	print join(" ", "null =", @null), "\n";
	my $M = &inverse($m);
	my $N = &inverse($n);
	my $G = &inverse($g);
	print "m=$m n=$n M=$M N=$N\n";
	my $commutator = &reduce("$m$n$M$N", @null);
	print "comm $commutator\n";
	@relations = ();
}
