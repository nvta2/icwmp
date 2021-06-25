#!/usr/bin/perl

use WWW::Mechanize;

#-----------------------------------------------------------------------------
# Globals variables
#-----------------------------------------------------------------------------
my $acs_script_url = "http://localhost:8080/openacs/scripts.jsf";
my $msg_to_sent = "";

#-----------------------------------------------------------------------------
# Fonctions
#-----------------------------------------------------------------------------
sub getRPCMethods { 
	my ($function, $repeat) = @_; 
	return ("cpe.GetRPCMethods();\n");
}

sub getParameterNames { 
	my ($function, $param, $bool, $repeat) = @_;
	return ("cpe.GetParameterNames('$param', $bool);\n");
}

sub get_param_value {	
	my ($function, $param_name, $req_num, $last) = @_;
	my $m = "";
	if ($req_num == 0) {
		$m = "$m"."var parameters = new Array();\n";	
	}
	$m = "$m"."parameters[$req_num] = '$param_name';\n";
	if ($last == 1) {
		$m = "$m"."var response = cpe.GetParameterValues(parameters);\n";
	}	
	return ($m);
}

sub set_param_value {
	my ($function, $param_name, $param_value, $req_num, $last) = @_;
	my $m = "";
	if ($req_num == 0) {	 
		$m = "$m"."var parameters = new Array();\n";
	}
	$m = "$m"."parameters[$req_num] = {name: '$param_name', value:'$param_value'};\n";
	if ($last == 1) {
		$m = "$m"."cpe.SetParameterValues(parameters, 'commandKey');\n";	
	}
	return ($m);
}

sub get_param_attribute {
	my ($function, $param_name, $req_num, $last) = @_;	
	my $m = "";
	if ($req_num == 0) {
		$m = "$m"."var p = new Array();\n";	
	}
	$m = "$m"."p[$req_num]='$param_name';\n";
	if ($last == 1) {
		$m = "$m"."var r = cpe.GetParameterAttributes(p);\n";
	}
	return ($m);
}

sub set_param_attribute {
	my ($function, $param_name, $param_notification_value, $req_num, $last) = @_;
	my $m = "";
	if ($req_num == 0) {
		$m = "$m"."var parameters = new Array();\n";	
	}
	$m = "$m"."parameters[$req_num]=new Object;\n";
	$m = "$m"."parameters[$req_num].Name='$param_name';\n";
	$m = "$m"."parameters[$req_num].Notification=$param_notification_value;\n";
	$m = "$m"."parameters[$req_num].NotificationChange=true;\n";
	$m = "$m"."parameters[$req_num].AccessListChange=true;\n";
	$m = "$m"."parameters[$req_num].AccessListChange=true;\n";
	$m = "$m"."parameters[$req_num].AccessList= new Array ();\n";
	$m = "$m"."parameters[$req_num].AccessList[0]='subscriber';\n";
	if ($last == 1) {
		$m = "$m"."cpe.SetParameterAttributes(parameters);\n";		
	}
	return ($m);
}

sub addobject {
	my ($function,$param,$commandkey) = @_;
	return ("cpe.AddObject('$param', '$commandkey');\n");
}

sub deleteobject {
	my ($function,$param,$commandkey) = @_;	
	return ("cpe.DeleteObject('$param', '$commandkey');\n");
}

sub scheduleInform {
	my ($function, $commandkey, $delay) = @_;
	return ("cpe.ScheduleInform($delay, '$commandkey');\n");
}

sub factoryReset {
	my ($function) = @_;
	return ("cpe.FactoryReset();\n");
}

sub download {
	my ($function, $commandnum, $username, $password, $file_size, $file_name, $delay, $repeat, $num_incr) = @_;
	my $m = "";
	my $url = "http://";
	for (my $i = 0; $i < $repeat; $i += 1) {
		if ($num_incr == 1) { 
			$commandnum = $i;
		}
		$m = "$m"."cpe.Download('$commandKey$commandnum', '$what_to_download','$url$file_name', $username, $password, $file_size, '', $delay, '', '');\n";
	}
	return ($m);
}

sub reboot {
	my ($function, $commandkey) = @_;
	return ("cpe.Reboot('$commandkey');\n");
}

#-----------------------------------------------------------------------------
# Main
#-----------------------------------------------------------------------------
if ( $#ARGV == -1) {
	$msg_to_sent = "";
} else {
	my $str2 = join(" ",@ARGV);
	$str2=~ s/\{\}//gs;
	my @value = split (/,/, $str2);
	for (@value) {
		s/\{\}//;	
	}
	foreach my $req (@value) {
		my @uni = split (/\\/, $req);
		my $j=0;
		my $last=0;
		foreach my $req1 (@uni) {
			my @tab = split(/\s/, $req1);
			if (($#uni == ($j)) || ($#uni == 0)) {
				$last=1;
			}

			if ( $tab[0] eq "GetRPCMethods") {
				$msg_to_sent = "$msg_to_sent".getRPCMethods(@tab);
			} elsif ( $tab[0] eq "GetParameterNames") {
				if ($tab[1] eq "") {
					push(@tab, "");
				}
				$msg_to_sent = "$msg_to_sent".getParameterNames(@tab);
			} elsif ($tab[0] eq "GetParameterValue") {
				if ($#tab == 2) {
					my $repeat_num = $tab[2];
					pop(@tab);
					for (my $k = 0; $k < $repeat_num - 1; $k += 1) {
						push(@tab, "$k", 0);
						$msg_to_sent = "$msg_to_sent".get_param_value(@tab);
						$j = $k+1;
						pop(@tab);
						pop(@tab);
					}				
				}
				if ($#tab == 0) {
					push(@tab, "", "$j", "$last");
				}
				push(@tab, "$j", "$last");
				$msg_to_sent = "$msg_to_sent".get_param_value(@tab);
			} elsif ($tab[0] eq "SetParameterValue") {
				push(@tab, "$j", "$last");
				$msg_to_sent ="$msg_to_sent".set_param_value(@tab);	
			} elsif ($tab[0] eq "GetParamAttribute") {
				if ($tab[1] eq "") {
					push(@tab, "", "$j", "$last");
				} else {
					push(@tab, "$j", "$last");
				}
				$msg_to_sent ="$msg_to_sent".get_param_attribute(@tab);
			} elsif ($tab[0] eq "SetParamAttribute") {
				if ($tab[1] eq "") {
					push(@tab, "");
				}
				push(@tab,"$j","$last");
				$msg_to_sent ="$msg_to_sent".set_param_attribute(@tab);
			} elsif ($tab[0] eq "AddObject") {
				if ($tab[1] eq "") {
					push(@tab, "", $tab[2], "$j","$last");
				} else {
					push(@tab, "$j", "$last");
				}
				$msg_to_sent = "$msg_to_sent".addobject(@tab);	
			} elsif ($tab[0] eq "DeleteObject") {
				if ($tab[1] eq "") {
					push(@tab, "", $tab[2], "$j", "$last");
				} else {
					push(@tab, "$j", "$last");
				}
				$msg_to_sent = "$msg_to_sent".deleteobject(@tab);
			} elsif ( $tab[0] eq "ScheduleInform") {
				$msg_to_sent = "$msg_to_sent".scheduleInform(@tab);
			} elsif ( $tab[0] eq "FactoryReset") {
				$msg_to_sent = "$msg_to_sent".factoryReset(@tab);
			} elsif ( $tab[0] eq "Download") {
				$msg_to_sent = "$msg_to_sent".download(@tab);
			} elsif ( $tab[0] eq "Reboot") {
				$msg_to_sent = "$msg_to_sent".reboot(@tab);
			}
			$j += 1
		}
		$j = 0;	
	}		 	
}

my $mech = WWW::Mechanize->new();
$mech->get($acs_script_url);
$mech->submit_form(form_number => 2, fields => {"scriptform:j_id60" => 'Default'}, button => 'scriptform:j_id69');
$mech->submit_form(form_number => 2, fields => {"scriptform:j_id62" => 'CWMP_Test', "scriptform:j_id64" => $msg_to_sent}, button => 'scriptform:j_id66');
print "message sent: $msg_to_sent";

