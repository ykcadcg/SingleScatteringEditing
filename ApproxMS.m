clear;
Rd = zeros(20, 20);
for r = 10: 10: 200
    i = 1; 
    j = 1;
    sigmasArray = 0.0: 0.5: 9.5; %20   %=sigmaS', since g = 0, isotropic;
    sigmaaArray = 0.0: 0.01: 0.19; %20
    for i = 1: 20
        for j = 1 : 20
            sigmas = sigmasArray(i);
            sigmaa = sigmaaArray(j);
            sigmat = sigmas + sigmaa;%=sigmat1
            alpha1 = sigmas ./sigmat;
            sigmatr = sqrt(3 * sigmaa .* sigmat);
            zr = 1 ./ sigmat;
            yita = 1.3;
            F = -1.440 / (yita*yita) + 0.710 / yita + 0.668 + 0.0636 * yita;
            A = (1 + F) / (1 - F);
            zv = zr .* (1 + 4 / 3 * A);
            dr = sqrt(r .* r + zr .* zr);
            dv = sqrt(r .* r + zv .* zv);
            PI = 3.1415926;
            left  = zr .* (sigmatr + 1./dr) .* exp(-sigmatr .* dr) ./ (dr .* dr);
            right = zv .* (sigmatr + 1./dv) .* exp(-sigmatr .* dv) ./ (dv .* dv);
            Rd(i, j) = alpha1 ./ (4 * PI) .* (left + right);
            sigmas
        end
    end
    surfc(Rd);
    xlabel('sigmaA');
    ylabel('sigmaS');
    zlabel('Rd');
    fn = ['r=', num2str(r), '.jpg'];
    saveas(gcf, fn);
    clf;
end
clear;


% clear;
% syms sigmas sigmaa  
% %sigmas = 0.5: 0.5: 10.0; %20   %=sigmaS', since g = 0, isotropic;
% %sigmaa = 0.01: 0.01:0.2; %20
% sigmat = sigmas + sigmaa;%=sigmat1
% alpha1 = sigmas ./sigmat;
% sigmatr = sqrt(3 * sigmaa .* sigmat);
% r = 10; %r = 10: 10: 200;
% zr = 1 ./ sigmat;
% yita = 1.3;
% F = 1.440 / (yita*yita) + 0.710 / yita + 0.668 + 0.0636 * yita;
% A = (1 + F) / (1 - F);
% zv = zr .* (1 + 4 / 3 * A);
% dr = sqrt(r .* r + zr .* zr);
% dv = sqrt(r .* r + zv .* zv);
% PI = 3.1415926;
% left  = zr .* (sigmatr + 1./dr) .* exp(-sigmatr .* dr) ./ (dr .* dr);
% right = zv .* (sigmatr + 1./dv) .* exp(-sigmatr .* dv) ./ (dv .* dv);
% Rd = alpha1 ./ (4 * PI) .* (left + right);
% 
% Realsigmas = [0.5: 0.5: 10.0];
% Realsigmaa = [0.01: 0.01:0.2];
% subs(Rd,{sigmas, sigmaa},{Realsigmas, Realsigmaa});
% surf(Realsigmas, Realsigmaa, Rd);
% clear;

